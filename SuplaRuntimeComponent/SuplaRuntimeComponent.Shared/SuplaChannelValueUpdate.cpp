/*
============================================================================
Name        : SuplaChannelValueUpdate.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/
#include "pch.h"
#include "SuplaChannelValueUpdate.h"

using namespace SuplaRuntimeComponent;

SuplaChannelValueUpdate::SuplaChannelValueUpdate()
{
}


Platform::Boolean SuplaChannelValueUpdate::getEOL() {
	return EOL;
}

int SuplaChannelValueUpdate::getId() {
	return Id;
}

Platform::Boolean SuplaChannelValueUpdate::getOnLine() {
	return OnLine;
}

SuplaChannelValue^ SuplaChannelValueUpdate::getValue() {
	return Value;
}

void SuplaChannelValueUpdate::setEOL(Platform::Boolean EOL) {
	this->EOL = EOL;
}

void SuplaChannelValueUpdate::setId(int Id) {
	this->Id = Id;
}

void SuplaChannelValueUpdate::setOnLine(Platform::Boolean OnLine) {
	this->OnLine = OnLine;
}

void SuplaChannelValueUpdate::setValue(SuplaChannelValue^ Value) {
	this->Value = Value;
}

