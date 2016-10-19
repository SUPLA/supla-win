/*
============================================================================
Name        : SuplaClient4Win_RTC.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

#include "pch.h"
#include "SuplaClient4Win_RTC.h"
#include "../../SuplaClient/SuplaClient.Shared/supla-client.h"

#include <Windows.h>

using namespace SuplaRuntimeComponent;
using namespace Platform;

_cb_on_action^ _onConnected;

typedef struct {
	
	HANDLE hThread;
	DWORD  dwThreadId;
	BOOL terminated;
	CRITICAL_SECTION critSec;
	SuplaClient4Win_RTC^ owner;

}TSC_ThreadData;

typedef struct {
	
	TSuplaClientCfg cfg;
	void *scd;
	TSC_ThreadData thread;

	int ClientID;
	
	_cb_on_action^ onConnecting;
	_cb_on_versionerror^ onVersionError;
	_cb_on_error^ onConnError;
	_cb_on_action^ onConnected;
	_cb_on_action^ onDisconnected;
	_cb_on_action^ onRegistering;
	_cb_on_registered^ onRegistered;
	_cb_on_error^ onRegisterError;
	_cb_location_update^ locationUpdate;
	_cb_channel_update^ channelUpdate;
	_cb_channel_update_value^ channelUpdateValue;
	_cb_on_event^ onEvent;

}TSC_Data;

#define sc_data ((TSC_Data*)_sc_data)

void suplaclient_cb_on_versionerror(void *_suplaclient, void *user_data, int version, int remote_version_min, int remote_version) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onVersionError) {
		sc_data->onVersionError(version, remote_version_min, remote_version);
	}

}

void suplaclient_cb_on_connerror(void *_suplaclient, void *user_data, int code) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onConnError) {
		sc_data->onConnError(code);
	}

}

void suplaclient_cb_on_connected(void *_suplaclient, void *user_data) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (_onConnected != nullptr) {
		int a = 10;
		a++;
	}

	if (sc_data
		&& sc_data->onConnected != nullptr ) {
		sc_data->onConnected();
	}

}

void suplaclient_cb_on_disconnected(void *_suplaclient, void *user_data) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onDisconnected != nullptr) {
		sc_data->onDisconnected();
	}

}

void suplaclient_cb_on_registering(void *_suplaclient, void *user_data) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onRegistering != nullptr) {
		sc_data->onRegistering();
	}

}

void suplaclient_cb_on_registered(void *_suplaclient, void *user_data, TSC_SuplaRegisterClientResult *result) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	sc_data->ClientID = result->ClientID;

	if (sc_data
		&& sc_data->onRegistered != nullptr) {

		SuplaRegisterResult^ r = ref new SuplaRegisterResult();

		r->setResultCode(result->result_code);
		r->setClientID(result->ClientID);
		r->setLocationCount(result->LocationCount);
		r->setChannelCount(result->ChannelCount);
		r->setActivityTimeout(result->activity_timeout);
		r->setVersion(result->version);
		r->setVersionMin(result->version_min);

		sc_data->onRegistered(r);
	}

}

void suplaclient_cb_on_registererror(void *_suplaclient, void *user_data, int code) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onRegisterError) {
		sc_data->onRegisterError(code);
	}

}

Platform::String^ suplaclient_GetString(char *src) {

	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);

	if (len > 0) {

		len += 2;
		wchar_t *wc = new wchar_t[len];

		memset(wc, 0, len);

		MultiByteToWideChar(CP_UTF8, 0, src, -1, wc, len);

		Platform::String^ result = ref new Platform::String(wc);
		delete[] wc;
		return result;
	}

	return "";
}

SuplaChannelValue^ suplaclient_GetChannelValue(TSuplaChannelValue *value) {

	if (value == NULL)
		return nullptr;

	auto v = ref new Array<uint8>(SUPLA_CHANNELVALUE_SIZE);
	int a;

	for (a = 0; a < SUPLA_CHANNELVALUE_SIZE; a++)
		v[a] = value->value[a];

	auto s = ref new Array<uint8>(SUPLA_CHANNELVALUE_SIZE);

	for (a = 0; a < SUPLA_CHANNELVALUE_SIZE; a++)
		s[a] = value->sub_value[a];

	SuplaChannelValue^ cvalue = ref new SuplaChannelValue();
	cvalue->setValue(v);
	cvalue->setSubValue(v);

	return cvalue;
}

void suplaclient_cb_location_update(void *_suplaclient, void *user_data, TSC_SuplaLocation *location) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->locationUpdate) {

		SuplaLocation^ l = ref new SuplaLocation();

		l->setEOL(location->EOL == 1 ? true : false);
		l->setId(location->Id);
		l->setCaption(suplaclient_GetString(location->Caption));

		sc_data->locationUpdate(l);
	}

}

void suplaclient_cb_channel_update(void *_suplaclient, void *user_data, TSC_SuplaChannel *channel) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->channelUpdate) {

		SuplaChannel^ c = ref new SuplaChannel();

		c->setEOL(channel->EOL == 1 ? true : false );
		c->setId(channel->Id);
		c->setLocationID(channel->LocationID);
		c->setFunc(channel->Func);
		c->setOnLine(channel->online == 1 ? true : false);
		c->setValue(suplaclient_GetChannelValue(&channel->value));
		c->setCaption(suplaclient_GetString(channel->Caption));

		sc_data->channelUpdate(c);
	}

}

void suplaclient_cb_channel_value_update(void *_suplaclient, void *user_data, TSC_SuplaChannelValue *channel_value) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->channelUpdateValue) {

		SuplaChannelValueUpdate^ vu = ref new SuplaChannelValueUpdate();

		vu->setEOL(channel_value->EOL == 1 ? true: false);
		vu->setId(channel_value->Id);
		vu->setOnLine(channel_value->online == 1 ? true : false);
		vu->setValue(suplaclient_GetChannelValue(&channel_value->value));

		sc_data->channelUpdateValue(vu);
	}

}

void suplaclient_cb_on_event(void *_suplaclient, void *user_data, TSC_SuplaEvent *event) {

	TSC_Data *_sc_data = (TSC_Data*)user_data;

	if (sc_data
		&& sc_data->onEvent) {

		SuplaEvent^ e = ref new SuplaEvent();

		e->setOwner(sc_data->ClientID == event->SenderID ? true : false);
		e->setEvent(event->Event);
		e->setChannelID(event->ChannelID);
		e->setDurationMS(event->DurationMS);
		e->setSenderID(event->SenderID);
		e->setSenderName(suplaclient_GetString(event->SenderName));


		sc_data->onEvent(e);
	}

}


BOOL SuplaClient4Win_isTerminated(TSC_ThreadData &td) {

	BOOL result = false;

	EnterCriticalSection(&td.critSec);
	result = td.terminated;
	LeaveCriticalSection(&td.critSec);

	return result;
}

void SuplaClient4Win_Terminate(TSC_ThreadData &td) {
	
	EnterCriticalSection(&td.critSec);
	td.terminated = true;
	LeaveCriticalSection(&td.critSec);

}

DWORD WINAPI SuplaClient4Win_ThreadFunction(LPVOID lpParam)
{

	TSC_Data* _sc_data = (TSC_Data*)lpParam;

	while (!SuplaClient4Win_isTerminated(sc_data->thread)) {

		if (sc_data->onConnecting != nullptr)
			sc_data->onConnecting();

		if (supla_client_connect(sc_data->scd)) {

		}

		while (!SuplaClient4Win_isTerminated(sc_data->thread)
			&& supla_client_iterate(sc_data->scd, 50)) {
		};


		if (!SuplaClient4Win_isTerminated(sc_data->thread)) {
			Sleep(2000);
		}

	}

	return 0;
}

SuplaClient4Win_RTC::SuplaClient4Win_RTC(void)
{

	_sc_data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TSC_Data));

	assert(_sc_data != NULL);

	memset(_sc_data, 0, sizeof(TSC_Data));

	InitializeCriticalSectionEx(&sc_data->thread.critSec, 0, 0);
	
	sc_data->scd = NULL;
	sc_data->thread.owner = this;

	supla_client_cfginit(&sc_data->cfg);

	sc_data->cfg.user_data = _sc_data;

	sc_data->cfg.cb_on_versionerror = &suplaclient_cb_on_versionerror;
	sc_data->cfg.cb_on_connerror = &suplaclient_cb_on_connerror;
	sc_data->cfg.cb_on_connected = &suplaclient_cb_on_connected;
	sc_data->cfg.cb_on_disconnected = &suplaclient_cb_on_disconnected;
	sc_data->cfg.cb_on_registering = &suplaclient_cb_on_registering;
	sc_data->cfg.cb_on_registered = &suplaclient_cb_on_registered;
	sc_data->cfg.cb_on_registererror = &suplaclient_cb_on_registererror;
	sc_data->cfg.cb_location_update = &suplaclient_cb_location_update;
	sc_data->cfg.cb_channel_update = &suplaclient_cb_channel_update;
	sc_data->cfg.cb_channel_value_update = &suplaclient_cb_channel_value_update;
	sc_data->cfg.cb_on_event = &suplaclient_cb_on_event;



}

BOOL SuplaClient4Win_RTC::GetMultibyteString(Platform::String^ Src, char *dest, int dest_len) {

	if (dest && dest_len > 0) {
		if (Src->Length() > 0) {
			wchar_t const *wc = Src->Data();
			memset(dest, 0, dest_len);
			return  WideCharToMultiByte(CP_UTF8, 0, wc, Src->Length(), dest, dest_len, NULL, NULL) > 0 ? true : false;
		}
		else {
			dest[0] = 0;
		}

	}


	return false;
}

char *SuplaClient4Win_RTC::GetMultibyteString(Platform::String^ Src) {

	wchar_t const *wc = Src->Data();
	char *result = NULL;

	if (Src->Length() > 0) {
		int len = WideCharToMultiByte(CP_UTF8, 0, wc, Src->Length(), NULL, 0, NULL, NULL);

		if (len > 0) {
			len++;
			result = new char[len];
			if (!GetMultibyteString(Src, result, len)) {
				delete[] result;
				result = NULL;
			};
		};
	}

	return result;
}

Platform::Boolean SuplaClient4Win_RTC::setSSLEnabled(Platform::Boolean Enabled) {
	
	if (sc_data->scd)
		return false;

	sc_data->cfg.ssl_enabled = Enabled;

	return true;
}

Platform::Boolean SuplaClient4Win_RTC::setHost(Platform::String^ Host) {

	if (sc_data->scd)
		return false;

	if (sc_data->cfg.host) {
		delete[] sc_data->cfg.host;
	}
	
	sc_data->cfg.host = GetMultibyteString(Host);
	return sc_data->cfg.host != NULL;
}

Platform::Boolean SuplaClient4Win_RTC::setPort(int Port) {

	if (sc_data->scd)
		return false;

	sc_data->cfg.tcp_port = Port;

	return true;
}

Platform::Boolean SuplaClient4Win_RTC::setAccessID(int AccessID) {

	if (sc_data->scd)
		return false;

	sc_data->cfg.AccessID = AccessID;

	return true;
}

Platform::Boolean SuplaClient4Win_RTC::setAccessIDpwd(Platform::String^ AccessIDpwd) {

	if (sc_data->scd)
		return false;

	return GetMultibyteString(AccessIDpwd, sc_data->cfg.AccessIDpwd, SUPLA_ACCESSID_PWD_MAXSIZE) ? true : false;
}

Platform::Boolean SuplaClient4Win_RTC::setClientGUID(const Platform::Array<uint8>^ GUID) {

	if (GUID->Length == SUPLA_GUID_SIZE) {

		memcpy(sc_data->cfg.clientGUID, GUID->Data, SUPLA_GUID_SIZE);
		return true;
	}

	return false;
}

Platform::Boolean SuplaClient4Win_RTC::setName(Platform::String^ Name) {

	if (sc_data->scd)
		return false;

	return GetMultibyteString(Name, sc_data->cfg.Name, SUPLA_CLIENT_NAME_MAXSIZE) ? true : false;
}

Platform::Boolean SuplaClient4Win_RTC::setSoftVer(Platform::String^ SoftVer) {

	if (sc_data->scd)
		return false;

	return GetMultibyteString(SoftVer, sc_data->cfg.SoftVer, SUPLA_SOFTVER_MAXSIZE) ? true : false;
}

Platform::Boolean SuplaClient4Win_RTC::setOnConnectingCallback(_cb_on_action^ onConnecting) {

	if (sc_data->scd)
		return false;

	sc_data->onConnecting = onConnecting;

	return true;
}

Platform::Boolean SuplaClient4Win_RTC::setOnConnectedCallback(_cb_on_action^ onConnected) {

	if (sc_data->scd)
		return false;

	sc_data->onConnected = onConnected;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnDisconnectedCallback(_cb_on_action^ onDisconnected) {

	if (sc_data->scd)
		return false;

	sc_data->onDisconnected = onDisconnected;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnRegisteringCallback(_cb_on_action^ onRegistering) {

	if (sc_data->scd)
		return false;

	sc_data->onRegistering = onRegistering;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnVersionErrorCallback(_cb_on_versionerror^ onVersionError) {

	if (sc_data->scd)
		return false;

	sc_data->onVersionError = onVersionError;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnConnErrorCallback(_cb_on_error^ onConnError) {

	if (sc_data->scd)
		return false;

	sc_data->onConnError = onConnError;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnRegisteredCallback(_cb_on_registered^ onRegistered) {

	if (sc_data->scd)
		return false;

	sc_data->onRegistered = onRegistered;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnRegisterErrorCallback(_cb_on_error^ onRegisterError) {


	if (sc_data->scd)
		return false;

	sc_data->onRegisterError = onRegisterError;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setLocationUpdateCallback(_cb_location_update^ locationUpdate) {

	if (sc_data->scd)
		return false;

	sc_data->locationUpdate = locationUpdate;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setChannelUpdateCallback(_cb_channel_update^ channelUpdate) {

	if (sc_data->scd)
		return false;

	sc_data->channelUpdate = channelUpdate;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setChannelUpdateValueCallback(_cb_channel_update_value^ channelUpdateValue) {

	if (sc_data->scd)
		return false;

	sc_data->channelUpdateValue = channelUpdateValue;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::setOnEventCallback(_cb_on_event^ onEvent) {

	if (sc_data->scd)
		return false;

	sc_data->onEvent = onEvent;

	return true;

}

Platform::Boolean SuplaClient4Win_RTC::open(int channelId, unsigned char _open) {

	if (!sc_data->scd)
		return false;

	return supla_client_open(sc_data->scd, channelId, _open) == 1 ? true : false;
}

Platform::Boolean SuplaClient4Win_RTC::setRGBW(int channelId, int color, unsigned char color_brightness, unsigned char brightness) {

	if (!sc_data->scd)
		return false;

	return supla_client_set_rgbw(sc_data->scd, channelId, color, color_brightness, brightness) == 1 ? true : false;
}

Platform::Boolean SuplaClient4Win_RTC::setDimmer(int channelId, unsigned char brightness) {

	if (!sc_data->scd)
		return false;

	return supla_client_set_dimmer(sc_data->scd, channelId, brightness) == 1 ? true : false;
}

int SuplaClient4Win_RTC::getCTID(void) {
	return GetCurrentThreadId();
}

void SuplaClient4Win_RTC::start(void) {

	if (sc_data->scd != NULL)
		return;


	sc_data->scd = supla_client_init(&sc_data->cfg);
	sc_data->thread.hThread = CreateThread(NULL, 0, SuplaClient4Win_ThreadFunction, sc_data, 0, &sc_data->thread.dwThreadId);

}

void SuplaClient4Win_RTC::stop(void) {

	SuplaClient4Win_Terminate(sc_data->thread);

	if (sc_data->thread.hThread) {
		WaitForSingleObjectEx(sc_data->thread.hThread, INFINITE, true);
		CloseHandle(sc_data->thread.hThread);
		sc_data->thread.hThread = NULL;
	}

	if (sc_data->scd != NULL) {
		supla_client_free(sc_data->scd);
		sc_data->scd = NULL;
	}


}

SuplaClient4Win_RTC::~SuplaClient4Win_RTC() {

	stop();

	DeleteCriticalSection(&sc_data->thread.critSec);

	if (sc_data->cfg.host != NULL) {
		delete[] sc_data->cfg.host;
		sc_data->cfg.host = NULL;
	}

	HeapFree(GetProcessHeap(), 0, _sc_data);

}
