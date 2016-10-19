/*
============================================================================
Name        : SuplaRegisterResult.cpp
Author      : Przemyslaw Zygmunt przemek@supla.org
Copyright   : GPLv2
============================================================================
*/

#include "pch.h"
#include "SuplaRegisterResult.h"

using namespace SuplaRuntimeComponent;

SuplaRegisterResult::SuplaRegisterResult()
{
}

int SuplaRegisterResult::getResultCode() {
	return ResultCode;
}

int SuplaRegisterResult::getClientID() {
	return ClientID;
}

int SuplaRegisterResult::getLocationCount() {
	return LocationCount;
}

int SuplaRegisterResult::getChannelCount() {
	return ChannelCount;
}

int SuplaRegisterResult::getActivityTimeout() {
	return ActivityTimeout;
}

int SuplaRegisterResult::getVersion() {
	return Version;
}

int SuplaRegisterResult::getVersionMin() {
	return VersionMin;
}

void SuplaRegisterResult::setResultCode(int ResultCode) {
	this->ResultCode = ResultCode;
}

void SuplaRegisterResult::setClientID(int ClientID) {
	this->ClientID = ClientID;
}

void SuplaRegisterResult::setLocationCount(int LocationCount) {
	this->LocationCount = LocationCount;
}

void SuplaRegisterResult::setChannelCount(int ChannelCount) {
	this->ChannelCount = ChannelCount;
}

void SuplaRegisterResult::setActivityTimeout(int ActivityTimeout) {
	this->ActivityTimeout = ActivityTimeout;
}

void SuplaRegisterResult::setVersion(int Version) {
	this->Version = Version;
}

void SuplaRegisterResult::setVersionMin(int VersionMin) {
	this->VersionMin = VersionMin;
}
