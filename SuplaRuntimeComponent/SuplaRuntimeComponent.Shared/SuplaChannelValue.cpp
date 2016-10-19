/*
============================================================================
Name        : SuplaChannelValue.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/
#include "pch.h"
#include "SuplaChannelValue.h"

using namespace SuplaRuntimeComponent;

SuplaChannelValue::SuplaChannelValue()
{
}

Platform::Array<uint8>^ SuplaChannelValue::getValue() {
	return ref new Platform::Array<uint8>(Value);
}

Platform::Array<uint8>^ SuplaChannelValue::getSubValue() {
	return ref new Platform::Array<uint8>(SubValue);
}

void SuplaChannelValue::setValue(const Platform::Array<uint8>^ Value) {
	this->Value = ref new Platform::Array<uint8>(Value);
}

void SuplaChannelValue::setSubValue(const Platform::Array<uint8>^ Value) {
	this->SubValue = ref new Platform::Array<uint8>(SubValue);
}


