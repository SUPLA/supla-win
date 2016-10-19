#pragma once
/*
============================================================================
Name        : SuplaChannelValue.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

#include "SuplaChannelValue.h"

namespace SuplaRuntimeComponent
{

	public ref class SuplaChannelValueUpdate sealed
	{
	private:

		Platform::Boolean EOL;
		int Id;
		Platform::Boolean OnLine;
		SuplaChannelValue^ Value;


	public:
		SuplaChannelValueUpdate();

		Platform::Boolean getEOL();
		int getId();
		Platform::Boolean getOnLine();
		SuplaChannelValue^ getValue();

		void setEOL(Platform::Boolean EOL);
		void setId(int Id);
		void setOnLine(Platform::Boolean OnLine);
		void setValue(SuplaChannelValue^ Value);
	};

}
