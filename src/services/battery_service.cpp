#include "Arduino.h"
#include "ArduinoBLE.h"

#include "./battery_service.h"

BLEService            batteryService("180F");
BLEByteCharacteristic batteryPercentage("2A19", BLERead);

void BatteryService::registerService() const
{
	Serial.println("Registering Battery Service");
	BLE.addService(batteryService);
}

void BatteryService::registerAttributes() const
{
	Serial.println("Registering Battery Service Attributes");
	batteryService.addCharacteristic(batteryPercentage);
	// TODO: actual battery readings
	batteryPercentage.writeValue(80);
}
