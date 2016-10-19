/*
============================================================================
Name        : SuplaEvent.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/
#include "pch.h"
#include "SuplaEvent.h"

using namespace SuplaRuntimeComponent;

SuplaEvent::SuplaEvent()
{
}


Platform::Boolean SuplaEvent::getOwner() {
	return Owner;
}

int SuplaEvent::getEvent() {
	return Event;
}

int SuplaEvent::getChannelID() {
	return ChannelID;
}

__int64 SuplaEvent::getDurationMS() {
	return DurationMS;
}

int SuplaEvent::getSenderID() {
	return SenderID;
}

Platform::String^ SuplaEvent::getSenderName() {
	return SenderName;
}

void SuplaEvent::setOwner(Platform::Boolean Owner) {
	this->Owner = Owner;
}

void SuplaEvent::setEvent(int Event) {
	this->Event = Event;
}

void SuplaEvent::setChannelID(int ChannelID) {
	this->ChannelID = ChannelID;
}

void SuplaEvent::setDurationMS(__int64 DurationMS) {
	this->DurationMS = DurationMS;
}

void SuplaEvent::setSenderID(int SenderID) {
	this->SenderID = SenderID;
}

void SuplaEvent::setSenderName(Platform::String^ SenderName) {
	this->SenderName = SenderName;
}

