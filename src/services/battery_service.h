#pragma once

#include "./base_service.h"

class BatteryService : public BaseService 
{
public:
	BatteryService() = default;

	void registerService()    const override;
	void registerAttributes() const override;
};
