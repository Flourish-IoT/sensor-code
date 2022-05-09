#include "ArduinoBLE.h"
#include "ctime"
#include "FlashStorage.h"

#include "./src/common.h"
#include "./src/communication/ble.h"
#include "./src/communication/wifi.h"
#include "./src/sensors/sensor_results.h"
#include "./src/sensors/sensor_operations.h"
#include "./src/services/commissioning_service.h"
#include "./src/services/commissioning_service.h"
#include "./src/services/wifi_service.h"
#include "./src/services/battery_service.h"
#include "./src/services/device_information_service.h"

#ifndef COMMISSION
#define COMMISSION true
#endif

extern SensorOperations * sensorOperations;

BluetoothCommissioner * commissioner = new BluetoothCommissioner(
	{
		new CommissioningService(COMMISSIONING_DEVICE_TYPE::SENSOR), 
		new WiFiService(), 
		new BatteryService(), 
		new DeviceInformationService()
	});

void setup()
{
	Serial.begin(SERIAL_RATE);
	while (!Serial);  // Wait for serial monitor to be open

	// initialize LED pins
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);

	commissioner->initialize();

#if COMMISSION
	if (commissioner->isInitialized())
#endif
		setupDevice();
#if COMMISSION
	else
		commissioner->startCommissioning();
#endif

	Serial.println("Setup complete");
}

void loop()
{
	switch (deviceState)
	{
		case DEVICE_STATE::COMMISSIONING: {
			BLEDevice central = BLE.central();

			digitalWrite(BLUE_LED, HIGH);

			if (central)
				while (central.connected())
					if (commissioner->execute() != 0)
						sensorOperations->onError();

			delay(500);
			digitalWrite(BLUE_LED, LOW);
			delay(500);
			break;
		}
		case DEVICE_STATE::COMMISSIONED:
			sensorOperations->readSensors();
			delay(DELAY_RATE);
			break;
		default:
			break;
	}
}
