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

	Serial.println("Reading sensors");
	StaticJsonDocument<JSON_SIZE> document;
	char data[JSON_SIZE];

	SensorResults::DHT11Results  * const dht11Results  = SensorOperations::readDHT11Sensor();
	SensorResults::SeesawResults * const seesawResults = SensorOperations::readSeesawSensor();
	SensorResults::LuxResults    * const luxResults    = SensorOperations::readLuxSensor();

	int8_t   const temperature  = dht11Results->temperature;
	uint8_t  const humidity     = dht11Results->humidity;
	uint16_t const capacitance  = seesawResults->capacitance;
	float    const luminescense = luxResults->luminescense;
	// TODO: when device is first turned on this returns 0 until it syncs with NTP server, need to wait for it to initialize
	ulong    const timestamp = WiFi.getTime();

	Serial.println("Temperature: " + String(temperature) + DEGREE_SYMBOL + "F");
	Serial.println("Humidity: " + String(humidity) + "%");
	Serial.println("Capacitance: " + String(capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luminescense) + " lux\n");
	Serial.println("Timestamp: " + String(timestamp));

	document["soilMoisture"]       = capacitance;
	document["light"]       			 = luminescense;
	document["humidity"]    			 = humidity;
	document["temperature"] 			 = temperature;
	document["timestamp"]        	 = timestamp;

	Serial.println("Sending data to server");
	serializeJson(document, data, JSON_SIZE);
	WifiOperations::PostResponse * const response = WifiOperations::postData(data);
	Serial.print("Status: ");
	Serial.print(response->status);
	if (response->status == 200) {
		clearError();
	} else {
		onError();
	}
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

void clearError() {
	digitalWrite(RED_LED, LOW);
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
			readSensors();
			delay(DELAY_RATE);
			break;
		default:
			break;
	}
}
