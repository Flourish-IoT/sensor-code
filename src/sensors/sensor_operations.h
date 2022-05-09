#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"
#include "DHT.h"

#include "./sensor_results.h"
#include "../common.h"

class SensorOperations
{
public:
	SensorOperations()
	{
		this->_humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
		this->_seesawSensor              = new Adafruit_seesaw();
		this->_luxSensor                 = new Adafruit_VEML7700();
	};

	inline void onError() const
	{
		digitalWrite(RED_LED, HIGH);
	}

	inline void clearError() const
	{
		digitalWrite(RED_LED, LOW);
	}

	void setupSensors() const;
	void readSensors()  const;
private:
	DHT               * _humidityTemperatureSensor;
	Adafruit_seesaw   * _seesawSensor;
	Adafruit_VEML7700 * _luxSensor;

	SensorResults::DHT11Results  * readDHT11Sensor()  const;
	SensorResults::SeesawResults * readSeesawSensor() const;
	SensorResults::LuxResults    * readLuxSensor()    const;
};
