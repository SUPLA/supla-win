/*
 ============================================================================
 Name        : supla-client.c
 Author      : Przemyslaw Zygmunt przemek@supla.org
 Copyright   : GPLv2
 ============================================================================
 */

#include <string.h>
#include <stdlib.h>

#include "supla-client.h"
#include "log.h"
#include "supla-socket.h"
#include "srpc.h"
#include "lck.h"

typedef struct {

	void *ssd;
	void *eh;
	void *srpc;
	void *lck;

	int client_id;

	struct timeval last_call;
	char connected;
	char registered;
	int server_activity_timeout;

	TSuplaClientCfg cfg;

}TSuplaClientData;

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

//http://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#endif

int supla_client_socket_read(void *buf, int count, void *scd) {
	return ssocket_read(((TSuplaClientData*)scd)->ssd, NULL, buf, count);
}

int supla_client_socket_write(void *buf, int count, void *scd) {
	return ssocket_write(((TSuplaClientData*)scd)->ssd, NULL, buf, count);
}

void supla_client_before_async_call(void *_srpc, unsigned _supla_int_t call_type, void *_scd) {

	TSuplaClientData *scd = (TSuplaClientData*)_scd;
	gettimeofday(&scd->last_call, NULL);

}

void supla_client_on_version_error(TSuplaClientData *scd, TSDC_SuplaVersionError *version_error) {

	supla_log(LOG_ERR, "Protocol version error. Server doesn't support this client. S:%d-%d/C:%d",
			version_error->server_version_min,
			version_error->server_version,
			SUPLA_PROTO_VERSION);


	if ( scd->cfg.cb_on_versionerror )
		scd->cfg.cb_on_versionerror(scd, scd->cfg.user_data, SUPLA_PROTO_VERSION, version_error->server_version_min, version_error->server_version);

	supla_client_disconnect(scd);
}

char supla_client_registered(void *_suplaclient) {
	TSuplaClientData *scd = (TSuplaClientData*)_suplaclient;
	char result;
	lck_lock(scd->lck);
	result = scd->registered;
	lck_unlock(scd->lck);

	return result;
}

void supla_client_set_registered(void *_suplaclient, char registered) {
	TSuplaClientData *scd = (TSuplaClientData*)_suplaclient;

	lck_lock(scd->lck);
	scd->registered = registered;
	lck_unlock(scd->lck);
}

void supla_client_on_register_result(TSuplaClientData *scd, TSC_SuplaRegisterClientResult *register_client_result) {

	if ( register_client_result->result_code == SUPLA_RESULTCODE_TRUE ) {

		supla_client_set_registered(scd, 1);

		scd->server_activity_timeout = register_client_result->activity_timeout;
		scd->client_id = register_client_result->ClientID;


		supla_log(LOG_DEBUG, "Registered.");

		if ( scd->cfg.cb_on_registered )
			scd->cfg.cb_on_registered(scd, scd->cfg.user_data, register_client_result);

	} else {

		switch(register_client_result->result_code) {
		case SUPLA_RESULTCODE_BAD_CREDENTIALS:
			supla_log(LOG_ERR, "Bad credentials!");
			break;

		case SUPLA_RESULTCODE_TEMPORARILY_UNAVAILABLE:
			supla_log(LOG_NOTICE, "Temporarily unavailable!");
			break;

		case SUPLA_RESULTCODE_ACCESSID_DISABLED:
			supla_log(LOG_NOTICE, "Access Identifier is disabled!");
			break;

		case SUPLA_RESULTCODE_CLIENT_DISABLED:
			supla_log(LOG_NOTICE, "Client is disabled!");
			break;

		case SUPLA_RESULTCODE_CLIENT_LIMITEXCEEDED:
			supla_log(LOG_NOTICE, "Client limit exceeded!");
			break;

		case SUPLA_RESULTCODE_GUID_ERROR:
			supla_log(LOG_NOTICE, "Incorrect client GUID!");
			break;

		}

		if ( scd->cfg.cb_on_registererror )
			scd->cfg.cb_on_registererror(scd, scd->cfg.user_data, register_client_result->result_code);

		supla_client_disconnect(scd);
	}

}

void supla_client_set_str(char *str, unsigned int *size, unsigned int max) {

	if ( *size > max )
		*size = max;

	if ( *size > 0 )
		str[(*size)-1] = 0;
	else
		str[0] = 0;

}

void supla_client_location_update(TSuplaClientData *scd, TSC_SuplaLocation *location, char gn) {

	supla_client_set_str(location->Caption, &location->CaptionSize, SUPLA_LOCATION_CAPTION_MAXSIZE);

	if ( scd->cfg.cb_location_update )
		scd->cfg.cb_location_update(scd, scd->cfg.user_data, location);

	if ( gn == 1 )
    	srpc_cs_async_get_next(scd->srpc);
}

void supla_client_locationpack_update(TSuplaClientData *scd, TSC_SuplaLocationPack *pack) {

	int a;

	for(a=0;a<pack->count;a++)
		supla_client_location_update(scd, &pack->locations[a], 0);

	srpc_cs_async_get_next(scd->srpc);
}

void supla_client_channel_update(TSuplaClientData *scd, TSC_SuplaChannel *channel, char gn) {

	supla_client_set_str(channel->Caption, &channel->CaptionSize, SUPLA_CHANNEL_CAPTION_MAXSIZE);

	if ( scd->cfg.cb_channel_update )
		scd->cfg.cb_channel_update(scd, scd->cfg.user_data, channel);

	if ( gn == 1 )
     	srpc_cs_async_get_next(scd->srpc);
}

void supla_client_channelpack_update(TSuplaClientData *scd, TSC_SuplaChannelPack *pack) {

	int a;

	for(a=0;a<pack->count;a++)
		supla_client_channel_update(scd, &pack->channels[a], 0);

	srpc_cs_async_get_next(scd->srpc);
}

void supla_client_channel_value_update(TSuplaClientData *scd, TSC_SuplaChannelValue *channel_value) {

	if ( scd->cfg.cb_channel_value_update )
		scd->cfg.cb_channel_value_update(scd, scd->cfg.user_data, channel_value);

	srpc_cs_async_get_next(scd->srpc);
}

void supla_client_on_event(TSuplaClientData *scd, TSC_SuplaEvent *event) {

	supla_client_set_str(event->SenderName, &event->SenderNameSize, SUPLA_SENDER_NAME_MAXSIZE);

	if ( scd->cfg.cb_on_event )
		scd->cfg.cb_on_event(scd, scd->cfg.user_data, event);

}

void supla_client_on_remote_call_received(void *_srpc, unsigned int rr_id, unsigned int call_type, void *_scd, unsigned char proto_version) {

	TsrpcReceivedData rd;
	char result;
	TSuplaClientData *scd = (TSuplaClientData*)_scd;



	if ( SUPLA_RESULT_TRUE == ( result = srpc_getdata(_srpc, &rd, 0)) ) {

		switch(rd.call_type) {
		case SUPLA_SDC_CALL_VERSIONERROR:
			supla_client_on_version_error(scd, rd.data.sdc_version_error);
			break;
		case SUPLA_SC_CALL_REGISTER_CLIENT_RESULT:
			supla_client_on_register_result(scd, rd.data.sc_register_client_result);
			break;
		case SUPLA_SC_CALL_LOCATION_UPDATE:
			supla_client_location_update(scd, rd.data.sc_location, 1);
			break;
		case SUPLA_SC_CALL_LOCATIONPACK_UPDATE:
			supla_client_locationpack_update(scd, rd.data.sc_location_pack);
			break;
		case SUPLA_SC_CALL_CHANNEL_UPDATE:
			supla_client_channel_update(scd, rd.data.sc_channel, 1);
			break;
		case SUPLA_SC_CALL_CHANNELPACK_UPDATE:
			supla_client_channelpack_update(scd, rd.data.sc_channel_pack);
			break;
		case SUPLA_SC_CALL_CHANNEL_VALUE_UPDATE:
			supla_client_channel_value_update(scd, rd.data.sc_channel_value);
			break;
		case SUPLA_SC_CALL_EVENT:
			supla_client_on_event(scd, rd.data.sc_event);
			break;
		}

		srpc_rd_free(&rd);

	} else if ( result == SUPLA_RESULT_DATA_ERROR ) {

		supla_log(LOG_DEBUG, "DATA ERROR!");
	}

}


void supla_client_cfginit(TSuplaClientCfg *sclient_cfg) {
	memset(sclient_cfg, 0, sizeof(TSuplaClientCfg));
	sclient_cfg->ssl_port = 2015;
	sclient_cfg->tcp_port = 2016;
	sclient_cfg->ssl_enabled = 1;
	#ifdef _WIN32
		sclient_cfg->iterate_wait_usec = 1000;
	#else
		sclient_cfg->iterate_wait_usec = 1000000;
	#endif
}

void *supla_client_init(TSuplaClientCfg *sclient_cfg) {

	TSuplaClientData *scd = malloc(sizeof(TSuplaClientData));
	memset(scd, 0, sizeof(TSuplaClientData));
	memcpy(&scd->cfg, sclient_cfg, sizeof(TSuplaClientCfg));

	scd->lck = lck_init();
	scd->cfg.AccessIDpwd[SUPLA_ACCESSID_PWD_MAXSIZE-1] = 0;
	scd->cfg.Name[SUPLA_CLIENT_NAME_MAXSIZE-1] = 0;
	scd->cfg.host = NULL;

	if (sclient_cfg->host != NULL
		&& strlen(sclient_cfg->host) > 0) {
		#ifdef _WIN32
				scd->cfg.host = _strdup(sclient_cfg->host);
		#else
				scd->cfg.host = strdup(sclient_cfg->host);
		#endif
	}

	scd->ssd = ssocket_client_init(scd->cfg.host, scd->cfg.ssl_enabled == 1 ? scd->cfg.tcp_port : scd->cfg.ssl_port,
			scd->cfg.ssl_enabled == 1);

	return scd;
}

void supla_client_clean(void *_suplaclient) {

	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;

	if ( suplaclient ) {

		if ( suplaclient->eh ) {
			eh_free(suplaclient->eh);
			suplaclient->eh = NULL;
		}

		if ( suplaclient->srpc ) {
			srpc_free(suplaclient->srpc);
			suplaclient->srpc = NULL;
		}
	}
}

void supla_client_free(void *_suplaclient) {

	if ( _suplaclient != NULL ) {


		supla_client_disconnect(_suplaclient);
		supla_client_clean(_suplaclient);

		TSuplaClientData *scd = (TSuplaClientData*)_suplaclient;


		if ( scd->cfg.host )
			free(scd->cfg.host);

		ssocket_free(scd->ssd);
		lck_free(scd->lck);


		free(_suplaclient);

	}

}


int supla_client_get_id(void *_suplaclient) {
	return ((TSuplaClientData*)_suplaclient)->client_id;
}

char supla_client_connected(void *_suplaclient) {
	return ((TSuplaClientData*)_suplaclient)->connected == 1;
}

void supla_client_disconnect(void *_suplaclient) {

	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;

	if ( supla_client_connected(_suplaclient) ) {


		suplaclient->connected = 0;

		supla_client_set_registered(_suplaclient, 0);

		ssocket_supla_socket__close(suplaclient->ssd);

		if ( suplaclient->cfg.cb_on_disconnected )
			suplaclient->cfg.cb_on_disconnected(_suplaclient, suplaclient->cfg.user_data);
	}

}

char supla_client_connect(void *_suplaclient) {

	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;
	supla_client_disconnect(_suplaclient);

	supla_client_clean(_suplaclient);

	int err = 0;

	if ( ssocket_client_connect(suplaclient->ssd, NULL, &err) == 1 ) {


		suplaclient->eh = eh_init();
		TsrpcParams srpc_params;
		srpc_params_init(&srpc_params);
		srpc_params.user_params = _suplaclient;
		srpc_params.data_read = &supla_client_socket_read;
		srpc_params.data_write = &supla_client_socket_write;
		srpc_params.on_remote_call_received = &supla_client_on_remote_call_received;
		srpc_params.before_async_call = &supla_client_before_async_call;
		srpc_params.eh = suplaclient->eh;
		suplaclient->srpc = srpc_init(&srpc_params);

		eh_add_fd(suplaclient->eh, ssocket_get_fd(suplaclient->ssd));
		suplaclient->connected = 1;

		supla_client_set_registered(_suplaclient, 0);

		if ( suplaclient->cfg.cb_on_connected )
			suplaclient->cfg.cb_on_connected(_suplaclient, suplaclient->cfg.user_data);

		return 1;

	} else {

		if ( suplaclient->cfg.cb_on_connerror )
			suplaclient->cfg.cb_on_connerror(_suplaclient, suplaclient->cfg.user_data, err);


	}

	return 0;
}

void supla_client_register(TSuplaClientData *suplaclient) {

	if ( suplaclient->cfg.cb_on_registering )
		suplaclient->cfg.cb_on_registering(suplaclient, suplaclient->cfg.user_data);


	TCS_SuplaRegisterClient src;
	memset(&src, 0, sizeof(TCS_SuplaRegisterClient));

	src.AccessID = suplaclient->cfg.AccessID;

#ifdef _WIN32
	_snprintf_s(src.AccessIDpwd, SUPLA_ACCESSID_PWD_MAXSIZE, _TRUNCATE, "%s", suplaclient->cfg.AccessIDpwd);
	_snprintf_s(src.Name, SUPLA_CLIENT_NAME_MAXSIZE, _TRUNCATE, "%s", suplaclient->cfg.Name);
	_snprintf_s(src.SoftVer, SUPLA_SOFTVER_MAXSIZE, _TRUNCATE, "%s", suplaclient->cfg.SoftVer);
#else
	snprintf(src.AccessIDpwd, SUPLA_ACCESSID_PWD_MAXSIZE, "%s", suplaclient->cfg.AccessIDpwd);
	snprintf(src.Name, SUPLA_CLIENT_NAME_MAXSIZE, "%s", suplaclient->cfg.Name);
	snprintf(src.SoftVer, SUPLA_SOFTVER_MAXSIZE, "%s", suplaclient->cfg.SoftVer);
#endif

	memcpy(src.GUID, suplaclient->cfg.clientGUID, SUPLA_GUID_SIZE);


	srpc_cs_async_registerclient(suplaclient->srpc, &src);
}

void supla_client_ping(TSuplaClientData *suplaclient) {

	struct timeval now;

	if ( suplaclient->server_activity_timeout > 0 ) {
		gettimeofday(&now, NULL);

		if ( (now.tv_sec-suplaclient->last_call.tv_sec) >= (suplaclient->server_activity_timeout-5) ) {
			srpc_dcs_async_ping_server(suplaclient->srpc);
		}
	}
}


char supla_client_iterate(void *_suplaclient, int wait_usec) {
	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;

	if ( supla_client_connected(_suplaclient) == 0 )
		return 0;

	if ( supla_client_registered(_suplaclient) == 0 ) {

		supla_client_set_registered(_suplaclient, -1);
		supla_client_register(suplaclient);

	} else if ( supla_client_registered(_suplaclient) == 1 ) {
		supla_client_ping(suplaclient);
	}


	if ( suplaclient->srpc != NULL
		 && srpc_iterate(suplaclient->srpc) == SUPLA_RESULT_FALSE ) {
		supla_client_disconnect(_suplaclient);
		return 0;
	}


	if ( supla_client_connected(_suplaclient) == 1
		 && suplaclient->eh != NULL ) {
		eh_wait(suplaclient->eh, wait_usec);
	}

	return 1;
}

void* supla_client_get_userdata(void *_suplaclient) {

	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;
	return suplaclient->cfg.user_data;
}

char supla_client_open(void *_suplaclient, int ChannelID, char open) {

	TCS_SuplaChannelNewValue_B value;
	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;
	char result = 0;

	lck_lock(suplaclient->lck);
	if ( supla_client_registered(_suplaclient) == 1 ) {

		memset(&value, 0, sizeof(TCS_SuplaChannelNewValue_B));
		value.ChannelId = ChannelID;
		value.value[0] = open;
		result =  srpc_cs_async_set_channel_value_b(suplaclient->srpc, &value) == SUPLA_RESULT_FALSE ? 0 : 1;

	}
	lck_unlock(suplaclient->lck);


	return result;
}

char supla_client_set_rgbw(void *_suplaclient, int ChannelID, int color, char color_brightness, char brightness) {

	TCS_SuplaChannelNewValue_B value;
	TSuplaClientData *suplaclient = (TSuplaClientData *)_suplaclient;
	char result = 0;

	lck_lock(suplaclient->lck);
	if ( supla_client_registered(_suplaclient) == 1 ) {

		memset(&value, 0, sizeof(TCS_SuplaChannelNewValue_B));
		value.ChannelId = ChannelID;
		value.value[0] = brightness;

		value.value[1] = color_brightness;
		value.value[2] = (char)((color & 0x000000FF));       // BLUE
		value.value[3] = (char)((color & 0x0000FF00) >> 8);  // GREEN
		value.value[4] = (char)((color & 0x00FF0000) >> 16); // RED

		result =  srpc_cs_async_set_channel_value_b(suplaclient->srpc, &value) == SUPLA_RESULT_FALSE ? 0 : 1;

	}
	lck_unlock(suplaclient->lck);


	return result;
}

char supla_client_set_dimmer(void *_suplaclient, int ChannelID, char brightness) {
	return supla_client_set_rgbw(_suplaclient, ChannelID, 0, 0, brightness);
}
