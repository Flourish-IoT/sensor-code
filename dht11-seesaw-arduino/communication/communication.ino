#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"
#include "DHT.h"
#include "Wire.h"
#include "ArduinoJson.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

uint8_t    const RED_LED_PIN          = 2;
uint8_t    const GREEN_LED_PIN        = 3;
uint8_t    const BLUE_LED_PIN         = 4;
uint8_t    const DIGITAL_PIN_7        = 7;
uint8_t    const SEESAW_I2C_ADDRESS   = 0x36;
uint8_t    const VEML7700_I2C_ADDRESS = 0x10;
uint16_t   const SERIAL_BAUD_RATE     = 115200;
uint16_t   const DELAY_RATE           = 1000;
uint16_t   const WIFI_PORT            = 80;
char       const DEGREE_SYMBOL        = 248;
char     * const MODEL                = "Flourish Device";
char     * const SERIAL_NUMBER        = "abcde";
char     * const HARDWARE_REVISION    = "0.1";
char     * const FIRMWARE_REVISION    = "0.1";
char     * const CONFIGURATOR_UUID    = "dabd-4a32-8e63-7631272ab6e3";
char     * const SCANNER_UUID         = "b50b-48b7-87e2-a6d52eb9cc9c";
char     * const CONTENT_TYPE         = "application/json";
char     * const SERVER_NAME          = "httpbin.org";
char     * const WIFI_SSID            = "";
char     * const WIFI_PASSWORD        = "";

DHT               * humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
Adafruit_seesaw   * seesawSensor              = new Adafruit_seesaw();
Adafruit_VEML7700 * luxSensor                 = new Adafruit_VEML7700();

String postData;
IPAddress ip;
WiFiClient wifi;

HttpClient              * const client   = new HttpClient(wifi, SERVER_NAME, WIFI_PORT);
// StaticJsonDocument<200> * const document = new StaticJsonDocument<200>();

int16_t status = WL_IDLE_STATUS;

namespace SensorResults {
	struct DHT11Results 
	{
		uint8_t humidity;                                                                                               // Humidity property is double, but as accurate as uint8_t
		int8_t temperature;                                                                                             // Temperature property is double, but as accurate as uint8_t
	};

	struct SeesawResults
	{
		float temperature;
		uint16_t capacitance;
	};

	struct LuxResults
	{
		float luminescense;
		float white;
		uint32_t ambientLight;
	};
}


SensorResults::DHT11Results * readDHT11Sensor() 
{
	SensorResults::DHT11Results * const dht11Results = (SensorResults::DHT11Results *) malloc(sizeof(SensorResults::DHT11Results));  // Allocate address for results struct

	dht11Results->humidity    = humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = humidityTemperatureSensor->readTemperature(true);
	
	return dht11Results;
} 

SensorResults::SeesawResults * readSeesawSensor() 
{
	SensorResults::SeesawResults * const seesawResults = (SensorResults::SeesawResults *) malloc(sizeof(SensorResults::SeesawResults));
	
	seesawResults->temperature = seesawSensor->getTemp();
	seesawResults->capacitance = seesawSensor->touchRead(0);
	
	return seesawResults;
}

SensorResults::LuxResults * readLuxSensor() 
{
	SensorResults::LuxResults * const luxResults = (SensorResults::LuxResults *) malloc(sizeof(SensorResults::LuxResults));

	luxResults->luminescense = luxSensor->readLux();
	luxResults->white = luxSensor->readWhite();
	luxResults->ambientLight = luxSensor->readALS();

	return luxResults;
}

void setup() {
	Serial.begin(SERIAL_BAUD_RATE);

	// #region Sensor Setup

	Wire.begin();

	while (!Serial);                                                                                               // Wait until the serial device responds
	Serial.println("Setting up sensors");

	if (!seesawSensor->begin(SEESAW_I2C_ADDRESS)) {
		Serial.println("CANNOT START SEESAW");
		for (;;) 
			delay(5000);
	}
	Serial.println("SEESAW STARTED");
	
	if (!luxSensor->begin()) {
		Serial.println("CANNOT START VEML");
	}
	Serial.println("VEML STARTED");

	Serial.println("Setting Lux Sensor Gain");
	luxSensor->setGain(VEML7700_GAIN_1_8);
	Serial.println("Setting Lux Sensor Integration Time");
	luxSensor->setIntegrationTime(VEML7700_IT_25MS);
	
	Serial.print("Beginning DHT11");
	humidityTemperatureSensor->begin();

	// #endregion

	// #region Wi-Fi Setup

	while (status != WL_CONNECTED) {
		Serial.println("Attempting to connect to network");
		status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		delay(5000);
	}

	Serial.println("Connected to network");
	
	// #endregion
}


void loop() {
	SensorResults::DHT11Results  * const dht11Results  = readDHT11Sensor();
	SensorResults::SeesawResults * const seesawResults = readSeesawSensor();
	SensorResults::LuxResults    * const luxResults    = readLuxSensor();

	int8_t   const temperature  = dht11Results->temperature;
	uint8_t  const humidity     = dht11Results->humidity;
	uint16_t const capacitance  = seesawResults->capacitance;
	float    const luminescense = luxResults->luminescense;

	Serial.println("Temperature: " + String(temperature) + DEGREE_SYMBOL + "F");
	Serial.println("Humidity: " + String(humidity) + "%");
	Serial.println("Capacitance: " + String(capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luminescense) + " lux\n");

	ip = WiFi.localIP();
	Serial.println("IP: " + String(ip));
	Serial.println("Signal strength (RSSI): " + String(WiFi.RSSI()));

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

	delay(DELAY_RATE);
}