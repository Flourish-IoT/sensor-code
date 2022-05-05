#include "./sensor_results.h"

namespace SensorOperations
{
	SensorResults::DHT11Results  * readDHT11Sensor();
	SensorResults::SeesawResults * readSeesawSensor();
	SensorResults::LuxResults    * readLuxSensor();
	void                           setupSensors();
}
