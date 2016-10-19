#pragma once
/*
============================================================================
Name        : SuplaLocation.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

namespace SuplaRuntimeComponent
{

	public ref class SuplaLocation sealed
	{
	private:
		Platform::Boolean EOL;
		int Id;
		Platform::String^ Caption;
	public:
		SuplaLocation();

		Platform::Boolean getEOL();
		int getId();
		Platform::String^ getCaption();

		void setEOL(Platform::Boolean EOL);
		void setId(int Id);
		void setCaption(Platform::String^ Caption);
	};

}

