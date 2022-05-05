#include "WiFiNINA.h"
#include "ArduinoBLE.h"
#include "FlashStorage.h"
#include "ArduinoJson.h"
#include "ArduinoHttpClient.h"

#include "src/common.h"
#include "src/communication/ble.h"
#include "src/sensors/sensor_results.h"
#include "src/sensors/sensor_operations.h"

String postData;
IPAddress ip;
WiFiClient wifi;
BLEDevice central;

int             deviceState  = DEVICE_STATE::IDLE;
char    * const SERVER_NAME  = "httpbin.org";
uint8_t   const WIFI_PORT    = 80;
char    * const CONTENT_TYPE = "application/json";
int16_t         status       = WL_IDLE_STATUS;

HttpClient * const client = new HttpClient(wifi, SERVER_NAME, WIFI_PORT);

inline void setupCommissioning() {
	Serial.println("Setting up commissioning");

	setupServices();
	deviceState = DEVICE_STATE::COMMISSIONING;
	startBle();

	Serial.println("Commissioning setup complete");
}

inline void setupDevice() {
	Serial.println("All services initialized, setting up device");
	deviceState = DEVICE_STATE::COMMISSIONED;
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);
}

void readSensors() {
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

	StaticJsonDocument<200> document;
	document["water"]       = capacitance;
	document["light"]       = luminescense;
	document["humidity"]    = humidity;
	document["temperature"] = temperature;
	document["time"]        = WiFi.getTime();

	serializeJson(document, postData);
	client->post("/post", CONTENT_TYPE, postData);

	status = client->responseStatusCode();
	Serial.print("Status Code: ");
	Serial.println(status);
	Serial.println(client->responseBody());
}

void setup()
{
	Serial.begin(9600);
	while (!Serial);

	// initialize pins
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);

	initializeServices();

	if (servicesInitialized()) {
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
		case DEVICE_STATE::COMMISSIONING: {
			BLEDevice central = BLE.central();

			digitalWrite(BLUE_LED, HIGH);

			if (central) {
				while (central.connected()) {
					executeServices();
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
			delay(1000);
			break;
		default:
			break;
	}
}
