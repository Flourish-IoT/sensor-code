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
