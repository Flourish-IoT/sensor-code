#include "Arduino.h"
#include "ArduinoBLE.h"
#include "WiFiNINA.h"
#include "vector"

#include "./ble.h"
#include "./wifi.h"
#include "../common.h"
#include "../services/base_service.h"
#include "../services/battery_service.h"
#include "../services/device_information_service.h"
#include "../services/commissioning_service.h"
#include "../services/wifi_service.h"

// TODO: vector not portable, switch to arrays
std::vector<BaseService *> services = { 
	new CommissioningService(COMMISSIONING_DEVICE_TYPE::SENSOR), 
	new WiFiService(), 
	new BatteryService(), 
	new DeviceInformationService(), 
};

void BluetoothOperations::onBLEConnected(BLEDevice const central) 
{
	Serial.print("Connected event, central: ");
	Serial.println(central.address());

	for (auto service : services)
		service->onBLEConnected();

	pinMode(BLUE_LED, HIGH);
}

void BluetoothOperations::onBLEDisconnected(BLEDevice const central) 
{
	Serial.print("Disconnected event, central: ");
	Serial.println(central.address());

	// TODO: cleanup

	for (auto service : services)
		service->onBLEDisconnected();
}

void BluetoothOperations::setupServices() 
{
	// setup characteristics
	for (auto service : services)
		service->registerAttributes();

	// setup event handlers
	BLE.setEventHandler(BLEConnected, BluetoothOperations::onBLEConnected);
	BLE.setEventHandler(BLEDisconnected, BluetoothOperations::onBLEDisconnected);
}

void BluetoothOperations::registerBleServices() 
{
	for (auto service : services)
		service->registerService();
}

void BluetoothOperations::startBle() 
{
	// stop wifi
	Serial.println("Stopping WiFi");
	WiFi.end();

	Serial.println("Initializing BLE");

	// initialize BLE
	if (!BLE.begin()) {
		Serial.println("Failed to start BLE");
		while (1);
	}

	BLE.setLocalName("Flourish Device");
	BLE.setDeviceName("Flourish Device");
	BLE.setAppearance(0x0540); // set appearance to Generic Sensor (from BLE appearance values)

	registerBleServices();

	BLE.advertise();
	Serial.println("BLE Initialized");
}

void BluetoothOperations::executeServices() 
{
	for (auto service : services)
		service->execute();
}

void BluetoothOperations::initializeServices() 
{
	for (auto service : services)
		service->initialize();
}

bool BluetoothOperations::servicesInitialized() 
{
	// makes sure every service is initialized
	for (auto service : services)
		if (!service->isInitialized())
			return false;

	return true;
}