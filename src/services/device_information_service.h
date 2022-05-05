#pragma once

#include "./base_service.h"

class DeviceInformationService : public BaseService 
{
public:
	DeviceInformationService() = default;

	void registerService()    const override;
	void registerAttributes() const override;
};
