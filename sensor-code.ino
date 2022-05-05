#include "ArduinoBLE.h"
#include "ArduinoJson.h"
#include "ctime"
#include "FlashStorage.h"

#include "./src/common.h"
#include "./src/communication/ble.h"
#include "./src/communication/wifi.h"
#include "./src/sensors/sensor_results.h"
#include "./src/sensors/sensor_operations.h"

BLEDevice central;

uint8_t deviceState = DEVICE_STATE::IDLE;

inline void setupCommissioning() 
{
	Serial.println("Setting up commissioning");

	BluetoothOperations::setupServices();
	deviceState = DEVICE_STATE::COMMISSIONING;
	BluetoothOperations::startBle();

	Serial.println("Commissioning setup complete");
}

inline void setupDevice() {
	Serial.println("All services initialized, setting up device");
	deviceState = DEVICE_STATE::COMMISSIONED;
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);
}

void readSensors() {
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

	digitalWrite(BLUE_LED, HIGH);

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
}

void setup()
{
	Serial.begin(SERIAL_RATE);
	while (!Serial);  // Wait for serial monitor to be open

	// initialize LED pins
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);

	BluetoothOperations::initializeServices();

	if (BluetoothOperations::servicesInitialized()) {
		setupDevice();
	} else {
		setupCommissioning();
	}

	Serial.println("Setup complete");
}


void loop()
{
	switch (deviceState)
	{
		case DEVICE_STATE::COMMISSIONING:
			central = BLE.central();

			digitalWrite(BLUE_LED, HIGH);

			if (central)
				while (central.connected())
					BluetoothOperations::executeServices();

			delay(DELAY_RATE);
			digitalWrite(BLUE_LED, LOW);
			delay(DELAY_RATE);
			break;
		case DEVICE_STATE::COMMISSIONED:
			Serial.println("Commisioned loop");
			readSensors();
			delay(DELAY_RATE);
			break;
		default:
			break;
	}
}
