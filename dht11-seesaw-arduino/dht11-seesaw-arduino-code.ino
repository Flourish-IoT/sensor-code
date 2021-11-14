/*
	Author      : David Bicer, Jr. -- dmjman
	File        : dht11-seesaw-arduino-code.ino
	Project     : Flourish
	Created On  : 2021-11-10
	Description : Program to read temperature, capacitance, and humidity from both the Inland DHT11 sensor and 
	              Adafruit Seesaw I2C Soil Moisture Sensor.
*/

#include <dht.h>                                                                                                        // Library for the DHT
#include "adafruit-seesaw.h"

uint8_t  const ANALOG_PIN_1          = A1;
uint8_t  const ANALOG_PIN_2          = A2;
uint8_t  const ANALOG_PIN_3          = A3;
uint8_t  const ANALOG_PIN_4          = A4;
uint8_t  const ANALOG_PIN_5          = A5;
uint8_t  const ANALOG_PIN_6          = A6;
uint8_t  const ANALOG_PIN_7          = A7;
uint8_t  const DIGITAL_PIN_7         = 7;
uint8_t  const SEESAW_CAPACITIVE_PIN = 0;
uint8_t  const SEESAW_I2C_ADDRESS    = 0x36;
uint16_t const SERIAL_BAUD_RATE      = 9600;                                                                            // 9600 bps should be plenty for our purposes
char     const DEGREE_SYMBOL         = 248;

dht            * humidityTemperatureSensor = new dht();                                                                 // Pointers use less memory than declared -- maybe able to write our own library
Seesaw::Seesaw * seesawSensor              = new Seesaw::Seesaw();

struct DHT11Results 
{
	uint8_t humidity;                                                                                               // Humidity property is double, but as accurate as uint8_t
	uint8_t temperature;                                                                                            // Temperature property is double, but as accurate as uint8_t
};

DHT11Results * readDHT11Sensor() 
{
	DHT11Results * const dht11Results                    = (DHT11Results *) malloc(sizeof(DHT11Results));           // Allocate address for results struct
	uint8_t        const humidityTemperatureSensorStatus = humidityTemperatureSensor->read11(DIGITAL_PIN_7);        // Our device is DHT 11, so use read11 method
	
	dht11Results->humidity    = humidityTemperatureSensor->humidity;                                                // Humidity and temperature stored in DHT object
	dht11Results->temperature = humidityTemperatureSensor->temperature; 

	return dht11Results;
} 

Seesaw::SeesawResults * readSeesawSensor() 
{
	Seesaw::SeesawResults * const seesawResults = (Seesaw::SeesawResults *) malloc(sizeof(Seesaw::SeesawResults));
	
	seesawResults->temperature = seesawSensor->getTemperature();
	seesawResults->capacitance = seesawSensor->getCapacitance();
	
	return seesawResults;
}

void setup()                                                                                                            // Runs once at beginning of code
{
	Serial.begin(SERIAL_BAUD_RATE);
	if (!seesawSensor->begin(SEESAW_I2C_ADDRESS)) {
		Serial.println("CANNOT START SEESAW");
		for (;;) 
			delay(5000);
	}
}

void loop()                                                                                                            // Runs continuously until device is manually stopped
{
	DHT11Results          * const dht11Results  = readDHT11Sensor();
	Seesaw::SeesawResults * const seesawResults = readSeesawSensor();

	Serial.println("Temperature: " + String(dht11Results->temperature) + DEGREE_SYMBOL + "C");
	Serial.println("Humidity: " + String(dht11Results->humidity) + "%");
	Serial.println("Capacitance: " + String(seesawResults->capacitance) + "/1024\n");

	delay(1000);                                                                                                   // Set loop interval in ms here
}
