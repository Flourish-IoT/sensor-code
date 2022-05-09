#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "WiFiNINA.h"

#include "./sensor_operations.h"
#include "./sensor_results.h"
#include "../common.h"
#include "../communication/wifi.h"


void SensorOperations::readSensors() const
{
	digitalWrite(BLUE_LED, HIGH);

	Serial.println("Reading sensors");
	StaticJsonDocument<JSON_SIZE> document;
	char data[JSON_SIZE];

	SensorResults::DHT11Results  * const dht11Results  = this->readDHT11Sensor();
	SensorResults::SeesawResults * const seesawResults = this->readSeesawSensor();
	SensorResults::LuxResults    * const luxResults    = this->readLuxSensor();

	int8_t   const temperature  = dht11Results->temperature;
	uint8_t  const humidity     = dht11Results->humidity;
	uint16_t const capacitance  = seesawResults->capacitance;
	float    const luminescense = luxResults->luminescense;

	uint32_t const timestamp    = WiFi.getTime();

	Serial.println("Temperature: " + String(temperature) + DEGREE_SYMBOL + "C");
	Serial.println("Humidity: " + String(humidity) + "%");
	Serial.println("Capacitance: " + String(capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luminescense) + " lux");
	Serial.println("Timestamp: " + String(timestamp) + "\n");

	document["soilMoisture"] = capacitance;
	document["light"]        = luminescense;
	document["humidity"]     = humidity;
	document["temperature"]  = temperature;
	document["timestamp"]    = timestamp;

	Serial.println("Sending data to server");
	serializeJson(document, data, JSON_SIZE);
	WifiOperations::PostResponse * const response = WifiOperations::postData(data);
	Serial.print("Status: ");
	Serial.print(response->status);
	if (response->status == 200)
		this->clearError();
	else
		this->onError();

	Serial.print(";\tResponse: ");
	Serial.println(response->body);

	digitalWrite(BLUE_LED, LOW);
}

void SensorOperations::setupSensors() const
{
	Serial.println("Setting up sensors");

	while (!this->_seesawSensor->begin(SEESAW_I2C_ADDRESS)) {
		Serial.println("CANNOT START SEESAW");
		delay(DELAY_RATE);
	}
	Serial.println("SEESAW STARTED");

	this->_humidityTemperatureSensor->begin();
	Serial.println("DHT11 STARTED");

	while (!this->_luxSensor->begin()) {
		Serial.println("CANNOT START VEML");
		delay(DELAY_RATE);
	}
	Serial.println("VEML STARTED");

	Serial.println("Setting Lux Sensor Gain");
	this->_luxSensor->setGain(VEML7700_GAIN_1_8);
	Serial.println("Setting Lux Sensor Integration Time");
	this->_luxSensor->setIntegrationTime(VEML7700_IT_25MS);
}


SensorResults::DHT11Results * SensorOperations::readDHT11Sensor() const
{
	SensorResults::DHT11Results * const dht11Results = (SensorResults::DHT11Results *) malloc(sizeof(SensorResults::DHT11Results));  // Allocate address for results struct

	dht11Results->humidity    = this->_humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = this->_humidityTemperatureSensor->readTemperature(false);                                             // true: F, false: C

	return dht11Results;
}

SensorResults::SeesawResults * SensorOperations::readSeesawSensor() const
{
	SensorResults::SeesawResults * const seesawResults = (SensorResults::SeesawResults *) malloc(sizeof(SensorResults::SeesawResults));

	seesawResults->temperature = this->_seesawSensor->getTemp();
	seesawResults->capacitance = this->_seesawSensor->touchRead(0);

	return seesawResults;
}

SensorResults::LuxResults * SensorOperations::readLuxSensor() const
{
	SensorResults::LuxResults * const luxResults = (SensorResults::LuxResults *) malloc(sizeof(SensorResults::LuxResults));

	luxResults->luminescense = this->_luxSensor->readLux();
	luxResults->white        = this->_luxSensor->readWhite();
	luxResults->ambientLight = this->_luxSensor->readALS();

	return luxResults;
}
