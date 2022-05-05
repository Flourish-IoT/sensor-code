#pragma once

namespace SensorResults 
{
	struct DHT11Results
	{
		char humidity;                                                                                                         // Humidity property is double, but as accurate as uint8_t
		char temperature;                                                                                                      // Temperature property is double, but as accurate as uint8_t
	};

	struct SeesawResults
	{
		float     temperature;
		short int capacitance;
	};

	struct LuxResults
	{
		float luminescense;
		float white;
		int   ambientLight;
	};
}
