#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"
#include "DHT.h"
#include "Wire.h"

uint8_t  const RED_LED_PIN           = 2;
uint8_t  const GREEN_LED_PIN         = 3;
uint8_t  const BLUE_LED_PIN          = 4;
uint8_t  const DIGITAL_PIN_7         = 7;
uint8_t  const SEESAW_I2C_ADDRESS    = 0x36;
uint8_t  const VEML7700_I2C_ADDRESS  = 0x10;
uint16_t const SERIAL_BAUD_RATE      = 115200;                                                                            // 9600 bps should be plenty for our purposes
uint16_t const DELAY_RATE            = 1000;
char     const DEGREE_SYMBOL         = 248;


DHT               * humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
Adafruit_seesaw   * seesawSensor              = new Adafruit_seesaw();
Adafruit_VEML7700 * luxSensor                 = new Adafruit_VEML7700();

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

struct LuxResults
{
	float luminescense;
	float white;
	uint32_t ambientLight;
};

DHT11Results * readDHT11Sensor() 
{
	DHT11Results * const dht11Results = (DHT11Results *) malloc(sizeof(DHT11Results));                               // Allocate address for results struct
  
	dht11Results->humidity    = humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = humidityTemperatureSensor->readTemperature(true);
	
	return dht11Results;
} 

SeesawResults * readSeesawSensor() 
{
	SeesawResults * const seesawResults = (SeesawResults *) malloc(sizeof(SeesawResults));
	
	seesawResults->temperature = seesawSensor->getTemp();
	seesawResults->capacitance = seesawSensor->touchRead(0);
	
	return seesawResults;
}

LuxResults * readLuxSensor() 
{
	LuxResults * const luxResults = (LuxResults *) malloc(sizeof(LuxResults));

	luxResults->luminescense = luxSensor->readLux();
	luxResults->white = luxSensor->readWhite();
	luxResults->ambientLight = luxSensor->readALS();

	return luxResults;
}

void findOpenI2CAddresses()
{
	uint8_t error;

	Serial.println("Scanning I2C devices");

	for (uint8_t address = 1; address < 127; ++address) {
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0 || error == 4)
			Serial.println("Address " + String(address) + ": " + String(error));
	}

	Serial.println("Done reading I2C devices");
}

void setup()                                                                                                           // Runs once at beginning of code
{
	Wire.begin();
	Serial.begin(SERIAL_BAUD_RATE);

	while (!Serial);                                                                                               // Wait until the serial device responds
	Serial.println("Setting up sensors");

	findOpenI2CAddresses();
	
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

void loop()                                                                                                           // Runs continuously until device is manually stopped
{
	DHT11Results  * const dht11Results  = readDHT11Sensor();
	SeesawResults * const seesawResults = readSeesawSensor();
	LuxResults    * const luxResults    = readLuxSensor();
	Serial.println("Temperature: " + String(dht11Results->temperature) + DEGREE_SYMBOL + "F");
	Serial.println("Humidity: " + String(dht11Results->humidity) + "%");
	Serial.println("Capacitance: " + String(seesawResults->capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luxResults->luminescense) + " lux\n");
	
	delay(DELAY_RATE);                                                                                                   // Set loop interval in ms here
}
