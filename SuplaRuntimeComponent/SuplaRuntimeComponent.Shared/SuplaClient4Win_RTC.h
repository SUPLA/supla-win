#pragma once

/*
============================================================================
Name        : SuplaClient4Win_RTC.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

#include "SuplaLocation.h"
#include "SuplaRegisterResult.h"
#include "SuplaChannel.h"
#include "SuplaChannelValueUpdate.h"
#include "SuplaEvent.h"

namespace SuplaRuntimeComponent
{

	public delegate void _cb_on_versionerror( int version, int remote_version_min, int remote_version);
	public delegate void _cb_on_error(int code);
	public delegate void _cb_on_action(void);
	public delegate void _cb_on_registered(SuplaRegisterResult^ registerResult);
	public delegate void _cb_location_update(SuplaLocation ^location);
	public delegate void _cb_channel_update(SuplaChannel ^channel);
	public delegate void _cb_channel_update_value(SuplaChannelValueUpdate ^channel_value);
	public delegate void _cb_on_event(SuplaEvent ^event);


	public ref class SuplaClient4Win_RTC sealed
	{
	private:
		~SuplaClient4Win_RTC();
		BOOL GetMultibyteString(Platform::String^ Src, char *dest, int dest_len);
		char *SuplaClient4Win_RTC::GetMultibyteString(Platform::String^ Src);
		
		void *_sc_data;



	public:
		SuplaClient4Win_RTC(void);

		Platform::Boolean setHost(Platform::String^ Host);
		Platform::Boolean setPort(int Port);
		Platform::Boolean setAccessID(int AccessID);
		Platform::Boolean setAccessIDpwd(Platform::String^ AccessIDpwd);
		Platform::Boolean setSSLEnabled(Platform::Boolean Enabled);
		Platform::Boolean setClientGUID(const Platform::Array<uint8>^ GUID);
		Platform::Boolean setName(Platform::String^ Name);
		Platform::Boolean setSoftVer(Platform::String^ SoftVer);

		Platform::Boolean setOnConnectingCallback(_cb_on_action^ onConnecting);
		Platform::Boolean setOnVersionErrorCallback(_cb_on_versionerror^ onVersionError);
		Platform::Boolean setOnConnErrorCallback(_cb_on_error^ onConnError);
		Platform::Boolean setOnConnectedCallback(_cb_on_action^ onConnected);
		Platform::Boolean setOnDisconnectedCallback(_cb_on_action^ onDisconnected);
		Platform::Boolean setOnRegisteringCallback(_cb_on_action^ onRegisterind);
		Platform::Boolean setOnRegisteredCallback(_cb_on_registered^ onRegistered);
		Platform::Boolean setOnRegisterErrorCallback(_cb_on_error^ onRegisterError);
		Platform::Boolean setLocationUpdateCallback(_cb_location_update^ locationUpdate);
		Platform::Boolean setChannelUpdateCallback(_cb_channel_update^ channelUpdate);
		Platform::Boolean setChannelUpdateValueCallback(_cb_channel_update_value^ channelUpdateValue);
		Platform::Boolean setOnEventCallback(_cb_on_event^ onEvent);

		Platform::Boolean open(int channelId, unsigned char _open);
		Platform::Boolean setRGBW(int channelId, int color, unsigned char color_brightness, unsigned char brightness);
		Platform::Boolean setDimmer(int channelId, unsigned char brightness);


		int getCTID(void);

		void start(void);
		void stop(void);
	};
}
