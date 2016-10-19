#pragma once
/*
============================================================================
Name        : SuplaChannel.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

#include "SuplaChannelValue.h"

namespace SuplaRuntimeComponent
{

	public ref class SuplaChannel sealed
	{
	private:

		Platform::Boolean EOL;
		int Id;
		int LocationID;
		int Func;
		Platform::Boolean OnLine;
		SuplaChannelValue^ Value;
		Platform::String^ Caption;

	public:
		SuplaChannel();

		Platform::Boolean getEOL();
		int getId();
		int getLocationID();
		int getFunc();
		Platform::Boolean getOnLine();
		SuplaChannelValue^ getValue();
		Platform::String^ getCaption();


		void setEOL(Platform::Boolean EOL);
		void setId(int Id);
		void setLocationID(int LocationID);
		void setFunc(int Func);
		void setOnLine(Platform::Boolean OnLine);
		void setValue(SuplaChannelValue^ Value);
		void setCaption(Platform::String^ Caption);

	};

}
