#pragma once
#include "base_service.h"

class BatteryService : public BaseService 
{
public:
	BatteryService() = default;

	void registerService() override;
	void registerAttributes() override;
};

class DeviceInformationService : public BaseService 
{
public:
	DeviceInformationService() = default;

	void registerService() override;
	void registerAttributes() override;
};
