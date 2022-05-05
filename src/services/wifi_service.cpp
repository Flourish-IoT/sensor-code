#include "Arduino.h"
#include "WiFiNINA.h"
#include "ArduinoBLE.h"
#include "FlashStorage.h"

#include "./wifi_service.h"
#include "../common.h"
#include "../communication/ble.h"
#include "../communication/wifi.h"

BLEService wifiService("00000000-b50b-48b7-87e2-a6d52eb9cc9c");
BLEUnsignedShortCharacteristic wifiState("00000001-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEWrite | BLEIndicate);
BLEStringCharacteristic wifiAPList("00000002-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEIndicate, BLE_MAX_CHARACTERISTIC_SIZE);
BLEStringCharacteristic wifiSsid("00000003-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEWrite | BLEIndicate, 32);
BLEStringCharacteristic wifiPassword("00000004-b50b-48b7-87e2-a6d52eb9cc9c", BLEWrite | BLEIndicate, 64);

void WiFiService::registerService() const
{
	Serial.println("Registering WiFi Service");
	BLE.addService(wifiService);
}

void WiFiService::registerAttributes() const
{
	Serial.println("Registering WiFi Service Attributes");
	wifiService.addCharacteristic(wifiState);
	wifiService.addCharacteristic(wifiAPList);
	wifiService.addCharacteristic(wifiSsid);
	wifiService.addCharacteristic(wifiPassword);

	wifiState.writeValue(WIFI_COMMISSIONING_STATE::IDLE);
}

int WiFiService::scanNetworks() const
{
	Serial.println("Starting WiFi scan");
	WifiOperations::startWifi();

	int8_t numSsid = WiFi.scanNetworks();
	Serial.println("Number of available networks: " + String(numSsid));

	if (numSsid == -1) {
		Serial.println("Couldn't get WiFi connection");
		// TODO: return error enum
		return -1;
	}

	// Serialize networks into string format
	// 2 					// network count
	// -85 Fios 	// rssi ssid
	// -88 Bar 		// rssi ssid
	Serial.println("Sending networks");
	String outputStr;
	outputStr.reserve(BLE_MAX_CHARACTERISTIC_SIZE);
	for (size_t i = 0; i < numSsid; i++)
	{
		String ssid(WiFi.SSID(i));
		String rssi(WiFi.RSSI(i));
		Serial.println("Network " + String(i) + ": " + ssid);
		Serial.println("Signal: " + rssi + " dBm");

		// string can't be larger than BLE max size (2 is size of space + newline)
		String newLine = "\n" + rssi + " " + ssid;
		if (outputStr.length() + newLine.length() > BLE_MAX_CHARACTERISTIC_SIZE) {
			Serial.println("Too many networks, truncating");
			break;
		}

		outputStr += newLine;
	}
	Serial.println("Output length: " + String(outputStr.length()));

	// restart ble and send available AP list
	BluetoothOperations::startBle();
	wifiAPList.writeValue(outputStr);

	Serial.println("Scan complete");
	return 0;
}

int WiFiService::joinNetwork() const
{
	Serial.println("Joining Network: " + wifiSsid.value());
	wifiState.writeValue(WIFI_COMMISSIONING_STATE::JOINING);

	WifiOperations::startWifi();

	Serial.println("Attempting to join");
	WiFi.setTimeout(10 * 1000);

	int wifiStatus = WiFi.begin(wifiSsid.value().c_str(), wifiPassword.value().c_str());

	int reasonCode = WiFi.reasonCode();
	if (wifiStatus != WL_CONNECTED) {
		Serial.println("Status: " + String(wifiStatus));
		Serial.println("reason: " + String(reasonCode)); // https://community.cisco.com/t5/wireless-mobility-documents/802-11-association-status-802-11-deauth-reason-codes/ta-p/3148055

		switch (wifiStatus) {
			case WL_FAILURE:
				Serial.println("WiFi failure");
				break;
			case WL_NO_SSID_AVAIL:
				Serial.println("Failed to join, SSID not available");
				break;
			case WL_CONNECT_FAILED:
				Serial.println("Failed to join");
				break;
			case WL_DISCONNECTED:
				Serial.println("Failed to join, incorrect password");
				break;
		}

		// TODO: handle error
		return -1;
	}

	BluetoothOperations::startBle();
	wifiState.writeValue(WIFI_COMMISSIONING_STATE::JOINED);

	return 0;
}

FlashStorage(networkStorage, Network);
int WiFiService::saveNetwork() 
{
	Serial.println("Saving network " + wifiSsid.value());

	this->_network = {
		wifiSsid.value(),
		wifiPassword.value()
	};

	networkStorage.write(this->_network);
	Serial.println("WiFi Information Saved");
	return 0;
}

int WiFiService::initialize() 
{
	Serial.println("Initializing WiFi service");
	Serial.println("Loading Network");
	this->_network = networkStorage.read();

	if (isInitialized()) {
		Serial.println("Loaded Network");
		Serial.println("SSID: " + this->_network.ssid);
		// TODO: setup wifi
	}
	return 0;
}

bool WiFiService::isInitialized() const
{
	return this->_network.ssid.length() > 0;
}

int WiFiService::execute() 
{
	if (!wifiState.written()) {
		return 0;
	}

	switch (wifiState.value()) {
		case WIFI_COMMISSIONING_STATE::SCAN:
			wifiState.writeValue(WIFI_COMMISSIONING_STATE::SCANNING);
			wifiState.writeValue(WiFiService::scanNetworks() ? WIFI_COMMISSIONING_STATE::SCANNED : WIFI_COMMISSIONING_STATE::ERROR);
			break;
		case WIFI_COMMISSIONING_STATE::SAVE:
			wifiState.writeValue(WIFI_COMMISSIONING_STATE::SAVING);
			wifiState.writeValue(WiFiService::saveNetwork() ? WIFI_COMMISSIONING_STATE::SAVED : WIFI_COMMISSIONING_STATE::ERROR);
			break;
		case WIFI_COMMISSIONING_STATE::JOIN:
			wifiState.writeValue(WIFI_COMMISSIONING_STATE::JOINING);
			wifiState.writeValue(WiFiService::joinNetwork() ? WIFI_COMMISSIONING_STATE::JOINED : WIFI_COMMISSIONING_STATE::ERROR);
			break;
		case WIFI_COMMISSIONING_STATE::ERROR:
			digitalWrite(RED_LED, HIGH);
			break;
		default: // idle
			break;
	}

	return 0;
}
