/*
============================================================================
Name        : SuplaLocation.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/
#include "pch.h"
#include "SuplaLocation.h"

using namespace SuplaRuntimeComponent;

SuplaLocation::SuplaLocation()
{
}

Platform::Boolean SuplaLocation::getEOL() {
	return EOL;
}

int SuplaLocation::getId() {
	return Id;
}

Platform::String^ SuplaLocation::getCaption() {
	return Caption;
}

void SuplaLocation::setEOL(Platform::Boolean EOL) {
	this->EOL = EOL;
}

void SuplaLocation::setId(int Id) {
	this->Id = Id;
}

void SuplaLocation::setCaption(Platform::String^ Caption) {
	this->Caption = Caption;
}

