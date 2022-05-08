#pragma once

#include "ArduinoBLE.h"

#include "./base_service.h"

namespace WIFI_COMMISSIONING_STATE
{
	enum {
		IDLE     = 1,

		SCAN     = 2,
		SCANNING = 4,
		SCANNED  = 8,

		JOIN     = 16,
		JOINING  = 32,
		JOINED   = 64,

		SAVE     = 128,
		SAVING   = 256,
		SAVED    = 512,

		ERROR    = 1024
	};
}

struct Network
{
	char ssid[32];
	char password[64];
};

class WiFiService : public BaseService
{
public:
	WiFiService() = default;

	void  registerService()    const override;
	void  registerAttributes() const override;
	int   execute()                  override;
	int   initialize()               override;
	bool  isInitialized()      const override;


private:
	Network network;

	int scanNetworks() const;
	int joinNetwork(String ssid, String password) const;
	int saveNetwork();
};
