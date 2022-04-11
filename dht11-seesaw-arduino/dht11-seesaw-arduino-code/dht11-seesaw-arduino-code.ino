#include "Adafruit_seesaw.h"
#include "DHT.h"

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

DHT             * humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
Adafruit_seesaw * seesawSensor              = new Adafruit_seesaw();


struct DHT11Results 
{
	uint8_t humidity;                                                                                               // Humidity property is double, but as accurate as uint8_t
	uint8_t temperature;                                                                                            // Temperature property is double, but as accurate as uint8_t
};

struct SeesawResults
{
	float temperature;
	uint16_t capacitance;
};

DHT11Results * readDHT11Sensor() 
{
	DHT11Results * const dht11Results = (DHT11Results *) malloc(sizeof(DHT11Results));                               // Allocate address for results struct
  
	dht11Results->humidity    = humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = humidityTemperatureSensor->readTemperature();
	
	return dht11Results;
} 

SeesawResults * readSeesawSensor() 
{
	SeesawResults * const seesawResults = (SeesawResults *) malloc(sizeof(SeesawResults));
	
	seesawResults->temperature = seesawSensor->getTemp();
	seesawResults->capacitance = seesawSensor->touchRead(0);
	
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
	humidityTemperatureSensor->begin();
}

void loop()                                                                                                            // Runs continuously until device is manually stopped
{
	DHT11Results  * const dht11Results  = readDHT11Sensor();
	SeesawResults * const seesawResults = readSeesawSensor();

	Serial.println("Temperature: " + String(dht11Results->temperature) + DEGREE_SYMBOL + "C");
	Serial.println("Humidity: " + String(dht11Results->humidity) + "%");
	Serial.println("Capacitance: " + String(seesawResults->capacitance) + "/1024\n");

	delay(1000);                                                                                                   // Set loop interval in ms here
}
