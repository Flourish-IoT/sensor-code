#include "Arduino.h"
#include "ArduinoBLE.h"
#include "WiFiNINA.h"

#include "./bluetooth_commissioner.h"
#include "./common.h"
#include "./communication/ble.h"

void onBLEConnected(BLEDevice central) 
{
	Serial.print("Connected event, central: ");
	Serial.println(central.address());

	for (auto service : commissioner->getServices())
		service->onBLEConnected();

	pinMode(BLUE_LED, HIGH);
}

void onBLEDisconnected(BLEDevice central) 
{
	Serial.print("Disconnected event, central: ");
	Serial.println(central.address());
	// TODO: cleanup

	for (auto service : commissioner->getServices())
		service->onBLEDisconnected();
}

void BluetoothCommissioner::setupServices() const
{
	// setup characteristics
	for (auto service : this->_services)
		service->registerAttributes();

	// setup event handlers
	BLE.setEventHandler(BLEConnected, onBLEConnected);
	BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);
}

void BluetoothCommissioner::registerServices() const
{
	for (auto service : this->_services)
		service->registerService();
}

void BluetoothCommissioner::startCommissioning() const
{
	Serial.println("Setting up commissioning");

	setupServices();
	deviceState = DEVICE_STATE::COMMISSIONING;
	// start BLE will register the services
	BluetoothOperations::startBle();

	Serial.println("Commissioning setup complete");
}

int BluetoothCommissioner::completeCommissioning() const
{
	Serial.println("Commissioning complete");

	initialize();

	if (isInitialized()) {
		setupDevice();
		// TODO: communicate with server to finalize device? need to be able to make the device status connected
		return 0;
	}

	return 1;
}

int BluetoothCommissioner::execute() const
{
	int result = 0;
	for (auto service : this->_services)
		result |= service->execute();

	return result == 0 ? 0 : 1;
}

int BluetoothCommissioner::initialize() const
{
	int result = 0;
	for (auto service : this->_services)
		result |= service->initialize();

	return result == 0 ? 0 : 1;
}

bool BluetoothCommissioner::isInitialized() const
{
	// makes sure every service is initialized
	for (auto service : this->_services)
		if (!service->isInitialized())
			return false;

	return true;
}