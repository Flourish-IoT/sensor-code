#include "Arduino.h"
#include "ArduinoBLE.h"
#include "WiFiNINA.h"
#include "vector"

#include "./ble.h"
#include "./wifi.h"
#include "../common.h"

namespace BluetoothOperations
{
	void startBle()
	{
		// stop wifi
		Serial.println("Stopping WiFi");
		WiFi.end();

		Serial.println("Initializing BLE");

		// initialize BLE
		if (!BLE.begin()) {
			Serial.println("Failed to start BLE");
			while (1);
		}

		BLE.setLocalName("Flourish Device");
		BLE.setDeviceName("Flourish Device");
		BLE.setAppearance(0x0540); // set appearance to Generic Sensor (from BLE appearance values)

		commissioner.registerServices();

		BLE.advertise();
		Serial.println("BLE Initialized");
	}
}
