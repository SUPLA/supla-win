#pragma once
/*
============================================================================
Name        : SuplaEvent.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

namespace SuplaRuntimeComponent
{

	public ref class SuplaEvent sealed
	{
	private:
		Platform::Boolean Owner;
		int Event;
		int ChannelID;
		__int64 DurationMS;
		int SenderID;
		Platform::String^ SenderName;
	public:
		SuplaEvent();

		Platform::Boolean getOwner();
		int getEvent();
		int getChannelID();
		__int64 getDurationMS();
		int getSenderID();
		Platform::String^ getSenderName();

		void setOwner(Platform::Boolean Owner);
		void setEvent(int Event);
		void setChannelID(int ChannelID);
		void setDurationMS(__int64 DurationMS);
		void setSenderID(int SenderID);
		void setSenderName(Platform::String^ SenderName);
	};

}

