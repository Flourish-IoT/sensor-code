#pragma once

#include "./sensor_results.h"
#include "./sensor_operations.h"
#include "../common.h"

#include "Arduino.h"
#include "DHT.h"
#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"

DHT               * const humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
Adafruit_seesaw   * const seesawSensor              = new Adafruit_seesaw();
Adafruit_VEML7700 * const luxSensor                 = new Adafruit_VEML7700();

SensorResults::DHT11Results * SensorOperations::readDHT11Sensor() 
{
	SensorResults::DHT11Results * const dht11Results = (SensorResults::DHT11Results *) malloc(sizeof(SensorResults::DHT11Results));  // Allocate address for results struct

	dht11Results->humidity    = humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = humidityTemperatureSensor->readTemperature(true);                                                    // true: F, false: C

	return dht11Results;
}

SensorResults::SeesawResults * SensorOperations::readSeesawSensor()
{
	SensorResults::SeesawResults * const seesawResults = (SensorResults::SeesawResults *) malloc(sizeof(SensorResults::SeesawResults));

	seesawResults->temperature = seesawSensor->getTemp();
	seesawResults->capacitance = seesawSensor->touchRead(0);

	return seesawResults;
}

SensorResults::LuxResults * SensorOperations::readLuxSensor()
{
	SensorResults::LuxResults * const luxResults = (SensorResults::LuxResults *) malloc(sizeof(SensorResults::LuxResults));

	luxResults->luminescense = luxSensor->readLux();
	luxResults->white        = luxSensor->readWhite();
	luxResults->ambientLight = luxSensor->readALS();

	return luxResults;
}

void SensorOperations::setupSensors()
{
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
}
