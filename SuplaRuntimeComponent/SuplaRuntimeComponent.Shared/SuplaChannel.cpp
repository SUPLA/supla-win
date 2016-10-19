/*
============================================================================
Name        : SuplaChannel.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/
#include "pch.h"
#include "SuplaChannel.h"

using namespace SuplaRuntimeComponent;

SuplaChannel::SuplaChannel()
{
}

Platform::Boolean SuplaChannel::getEOL() {
	return EOL;
}

int SuplaChannel::getId() {
	return Id;
}

int SuplaChannel::getLocationID() {
	return LocationID;
}

int SuplaChannel::getFunc() {
	return Func;
}

Platform::Boolean SuplaChannel::getOnLine() {
	return OnLine;
}

SuplaChannelValue^ SuplaChannel::getValue() {
	return Value;
}

Platform::String^ SuplaChannel::getCaption() {
	return Caption;
}


void SuplaChannel::setEOL(Platform::Boolean EOL) {
	this->EOL = EOL;
}

void SuplaChannel::setId(int Id) {
	this->Id = Id;
}

void SuplaChannel::setLocationID(int LocationID) {
	this->LocationID = LocationID;
}

void SuplaChannel::setFunc(int Func) {
	this->Func = Func;
}

void SuplaChannel::setOnLine(Platform::Boolean OnLine) {
	this->OnLine = OnLine;
}

void SuplaChannel::setValue(SuplaChannelValue^ Value) {
	this->Value = Value;
}

void SuplaChannel::setCaption(Platform::String^ Caption) {
	this->Caption = Caption;
}


