#include "./common.h"
#include "./sensors/sensor_operations.h"

uint8_t deviceState = DEVICE_STATE::IDLE;
DeviceInformation deviceInformation;

void setupDevice()
{
	Serial.println("All services initialized, setting up device");
	SensorOperations::setupSensors();
	deviceState = DEVICE_STATE::COMMISSIONED;
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(GREEN_LED, HIGH);
}
