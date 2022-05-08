#include "ArduinoBLE.h"
#include "ArduinoJson.h"
#include "ctime"
#include "FlashStorage.h"

#include "./src/common.h"
#include "./src/communication/ble.h"
#include "./src/communication/wifi.h"
#include "./src/sensors/sensor_results.h"
#include "./src/sensors/sensor_operations.h"

#include "src/services/commissioning_service.h"
#include "src/services/commissioning_service.h"
#include "src/services/wifi_service.h"
#include "src/services/battery_service.h"
#include "src/services/device_information_service.h"

#ifndef COMMISSION
#define COMMISSION true
#endif

BluetoothCommissioner commissioner = BluetoothCommissioner({new CommissioningService(COMMISSIONING_DEVICE_TYPE::SENSOR), new WiFiService(), new BatteryService(), new DeviceInformationService()});

void readSensors()
{
	digitalWrite(BLUE_LED, HIGH);

	StaticJsonDocument<200> document;
	char * data;

	SensorResults::DHT11Results  * const dht11Results  = SensorOperations::readDHT11Sensor();
	SensorResults::SeesawResults * const seesawResults = SensorOperations::readSeesawSensor();
	SensorResults::LuxResults    * const luxResults    = SensorOperations::readLuxSensor();

	int8_t   const temperature  = dht11Results->temperature;
	uint8_t  const humidity     = dht11Results->humidity;
	uint16_t const capacitance  = seesawResults->capacitance;
	float    const luminescense = luxResults->luminescense;

	Serial.println("Temperature: " + String(temperature) + DEGREE_SYMBOL + "F");
	Serial.println("Humidity: " + String(humidity) + "%");
	Serial.println("Capacitance: " + String(capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luminescense) + " lux\n");


	document["water"]       = capacitance;
	document["light"]       = luminescense;
	document["humidity"]    = humidity;
	document["temperature"] = temperature;
	document["time"]        = (float) std::time(nullptr);

	size_t size = sizeof(data) / sizeof(char);

	serializeJson(document, data, size);
	WifiOperations::PostResponse * const response = WifiOperations::postData(data);
	Serial.print("Status: ");
	Serial.print(response->status);
	Serial.print(";\tResponse: ");
	Serial.println(response->body);

	digitalWrite(BLUE_LED, LOW);
}

void setup()
{
	Serial.begin(SERIAL_RATE);
	while (!Serial);  // Wait for serial monitor to be open

	// initialize LED pins
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);

	commissioner.initialize();

#if COMMISSION
	if (commissioner.isInitialized())
#endif
	{
		setupDevice();
	}
#if COMMISSION
	else
	{
		commissioner.startCommissioning();
	}
#endif

	Serial.println("Setup complete");
}

void onError() {
	digitalWrite(RED_LED, HIGH);
}

void loop()
{
	switch (deviceState)
	{
		case DEVICE_STATE::COMMISSIONING: {
			BLEDevice central = BLE.central();

			digitalWrite(BLUE_LED, HIGH);

			if (central) {
				while (central.connected()) {
					if (commissioner.execute() != 0) {
						onError();
					}
				}
			}

			delay(500);
			digitalWrite(BLUE_LED, LOW);
			delay(500);
			break;
		}
		case DEVICE_STATE::COMMISSIONED:
			Serial.println("Commisioned loop");
			readSensors();
			delay(DELAY_RATE);
			break;
		default:
			break;
	}
}
