#pragma once

#include "./base_service.h"

struct DeviceInformation
{
	unsigned int deviceId;
	String       deviceToken;
	String       name;
};

class CommissioningService : public BaseService 
{
public:
	CommissioningService(int const deviceType);
	
	void registerService()    const override;
	void registerAttributes() const override;
	int  initialize()               override;
	bool isInitialized()      const override;
	int  execute()                  override;

private:
	int               _deviceType;
	DeviceInformation _deviceInfo;

	int saveDeviceInformation() const;
};

namespace COMMISSIONING_STATE 
{
	enum {
		IDLE     = 1,
		SAVE     = 2,
		SAVING   = 4,
		SAVED    = 8,
		COMPLETE = 16,
		ERROR    = 1024 
	};
}

namespace COMMISSIONING_DEVICE_TYPE 
{
	enum {
		SENSOR  = 1,
		GATEWAY = 2,
		OTHER   = 512
	};
}
