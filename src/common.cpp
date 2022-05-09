#include "./common.h"
#include "./sensors/sensor_operations.h"

uint8_t deviceState = DEVICE_STATE::IDLE;
DeviceInformation deviceInformation;

SensorOperations * sensorOperations = new SensorOperations();

void setupDevice()
{
	Serial.println("All services initialized, setting up device");
	sensorOperations->setupSensors();
	deviceState = DEVICE_STATE::COMMISSIONED;
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);
}
