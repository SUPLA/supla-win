#pragma once
/*
============================================================================
Name        : SuplaChannelValue.h
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

namespace SuplaRuntimeComponent
{

	public ref class SuplaChannelValue sealed
	{
	private:
		Platform::Array<uint8>^ Value;
		Platform::Array<uint8>^ SubValue;
	public:
		SuplaChannelValue();

		Platform::Array<uint8>^ getValue();
		Platform::Array<uint8>^ getSubValue();

		void setValue(const Platform::Array<uint8>^ Value);
		void setSubValue(const Platform::Array<uint8>^ Value);
	};

}

