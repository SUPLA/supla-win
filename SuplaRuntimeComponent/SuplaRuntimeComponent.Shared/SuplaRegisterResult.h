#pragma once
/*
============================================================================
Name        : SuplaRegisterResult.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

namespace SuplaRuntimeComponent
{

	public ref class SuplaRegisterResult sealed
	{

	private:
		
		int ResultCode;
		int ClientID;
		int LocationCount;
		int ChannelCount;
		int ActivityTimeout;
		int Version;
		int VersionMin;

	public:
		SuplaRegisterResult();

		int getResultCode();
		int getClientID();
		int getLocationCount();
		int getChannelCount();
		int getActivityTimeout();
		int getVersion();
		int getVersionMin();

		void setResultCode(int ResultCode);
		void setClientID(int ClientID);
		void setLocationCount(int LocationCount);
		void setChannelCount(int ChannelCount);
		void setActivityTimeout(int ActivityTimeout);
		void setVersion(int Version);
		void setVersionMin(int VersionMin);
	};

}
