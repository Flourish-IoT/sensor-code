#pragma once

#include "Arduino.h"
#include "ArduinoBLE.h"
#include "WiFiNINA.h"

namespace WifiOperations
{
	struct PostResponse
	{
		uint16_t   status;
		char     * body;
	};

	void startWifi();
	PostResponse * postData(char * const);
}