#include "Adafruit_seesaw.h"
#include "Adafruit_VEML7700.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "ArduinoJson.h"
#include "DHT.h"
#include "FlashStorage.h"
#include "WiFiNINA.h"
#include "Wire.h"

#include <map>

uint8_t    const RED_LED_PIN          = 2;
uint8_t    const GREEN_LED_PIN        = 3;
uint8_t    const BLUE_LED_PIN         = 4;
uint8_t    const DIGITAL_PIN_7        = 7;
uint8_t    const SEESAW_I2C_ADDRESS   = 0x36;
uint8_t    const VEML7700_I2C_ADDRESS = 0x10;
uint16_t   const SERIAL_BAUD_RATE     = 115200;
uint16_t   const DELAY_RATE           = 1000;
uint16_t   const WIFI_PORT            = 80;
char       const DEGREE_SYMBOL        = 248;
char     * const MODEL                = "Flourish Device";
char     * const SERIAL_NUMBER        = "abcde";
char     * const HARDWARE_REVISION    = "0.1";
char     * const FIRMWARE_REVISION    = "0.1";
char     * const CONTENT_TYPE         = "application/json";
char     * const SERVER_NAME          = "httpbin.org";
char     * const WIFI_SSID            = "Fios-5cJfH";
char     * const WIFI_PASSWORD        = "vane29waft98fan";

DHT               * const humidityTemperatureSensor = new DHT(DIGITAL_PIN_7, DHT11);
Adafruit_seesaw   * const seesawSensor              = new Adafruit_seesaw();
Adafruit_VEML7700 * const luxSensor                 = new Adafruit_VEML7700();

String postData;
IPAddress ip;
WiFiClient wifi;
BLEDevice central;

HttpClient * const client = new HttpClient(wifi, SERVER_NAME, WIFI_PORT);

bool    wifiMode = false;
int16_t status   = WL_IDLE_STATUS;

namespace SensorResults 
{
	struct DHT11Results
	{
		uint8_t humidity;                                                                                                        // Humidity property is double, but as accurate as uint8_t
		int8_t temperature;                                                                                                      // Temperature property is double, but as accurate as uint8_t
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
}

namespace DeviceCommissioning
{
	struct Network
	{
		int32_t rssi;
		uint8_t encryptionType;
		char    ssid[32];
	};

	static constexpr const uint16_t MAX_PACKET_NETWORK_COUNT = (512 - sizeof(int8_t))/sizeof(Network);

	struct PersistentInfo
	{
		char     * ssid;
		char     * password;
		uint32_t   deviceId;
		uint8_t    encryptionType;
	};

	struct NetworkPacket 
	{
		int8_t count;
		Network networks[MAX_PACKET_NETWORK_COUNT];
	};


	enum {
		IDLE          = 1,
		SCAN          = 2,
		SAVE          = 2,
		SCANNING      = 4,
		SAVING        = 4,
		SCANNED       = 8,
		SAVED         = 8,
		JOIN          = 16,
		SCANNER_ERROR = 16,
		JOINING       = 32,
		JOINED        = 64,
		CONFIG_ERROR  = 128
	};

	std::map<int, String> encryptionTypeMap = {
		{ ENC_TYPE_WEP, "WEP" },
		{ ENC_TYPE_TKIP, "WPA" },
		{ ENC_TYPE_CCMP, "WPA2" },
		{ ENC_TYPE_NONE, "None" },
		{ ENC_TYPE_AUTO, "Auto" }
	};

	void startWifi()
	{
		wifiMode = true;

		Serial.println("Stopping BLE");
		BLE.stopAdvertise();
		BLE.stopScan();
		BLE.end();

		Serial.println("Initializing WiFi");
		wiFiDrv.wifiDriverDeinit();
		wiFiDrv.wifiDriverInit();
		status = WL_IDLE_STATUS;

		delay(DELAY_RATE);                                                                                                       // Give driver time to start

		Serial.println("WiFi initialized");
	}

	void onBLEConnected(BLEDevice central)
	{
		Serial.print("Connected event, central: ");
		Serial.println(central.address());

		wifiScannerScanState.writeValue(IDLE);
		wifiConfigState.writeValue(IDLE);

		batteryPercentage.writeValue(80);
		pinMode(BLUE_LED_PIN, HIGH);
	}

	void onBLEDisconnected(BLEDevice central)
	{
		Serial.print("Disconnected event, central: ");
		Serial.println(central.address());
	}

	void startBle()
	{
		wifiMode = false;

		Serial.println("Initializing BLE");

		// initialize BLE
		if (!BLE.begin()) {
			Serial.println("Failed to start BLE");
			while (1);
		}

		Serial.println("BLE Initialized");

		// TODO: user configurable name?
		BLE.setLocalName("Flourish Device");
		BLE.setDeviceName("Flourish Device");
		BLE.setAdvertisedService(wifiScannerService);

		// setup BLE services and characteristics
		BLE.addService(wifiScannerService);
		BLE.addService(wifiConfiguratorService);
		BLE.addService(batteryService);
		
		deviceManufacturerName.writeValue("Flourish");
		deviceModelNumber.writeValue(MODEL);
		deviceSerialNumber.writeValue(SERIAL_NUMBER);
		deviceHardwareRevision.writeValue(HARDWARE_REVISION);
		deviceFirmwareRevision.writeValue(FIRMWARE_REVISION);
		BLE.addService(deviceInformationService);

		// setup event handlers
		BLE.setEventHandler(BLEConnected, onBLEConnected);
		BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);

		BLE.setAppearance(0x0540);                                                                                               // set appearance to Generic Sensor (from BLE appearance values)

		BLE.advertise();
	}
}

BLEService              batteryService("180F");
BLEByteCharacteristic   batteryPercentage("2A19", BLERead);
BLEService              deviceInformationService("180A");
BLEStringCharacteristic deviceManufacturerName("2A29", BLERead, 20);
BLEStringCharacteristic deviceModelNumber("2A24", BLERead, 20);
BLEStringCharacteristic deviceSerialNumber("2A25", BLERead, 20);
BLEStringCharacteristic deviceHardwareRevision("2A27", BLERead, 20);
BLEStringCharacteristic deviceFirmwareRevision("2A26", BLERead, 20);
BLEService              wifiScannerService("00000000-b50b-48b7-87e2-a6d52eb9cc9c");
BLEByteCharacteristic   wifiScannerScanState("00000001-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEWrite | BLEIndicate);
BLECharacteristic       wifiScannerAPList("00000002-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEIndicate, sizeof(DeviceCommissioning::NetworkPacket));
BLEByteCharacteristic   wifiScannerPacketCount("00000003-b50b-48b7-87e2-a6d52eb9cc9c", BLERead | BLEIndicate);
BLEService              wifiConfiguratorService("00000000-dabd-4a32-8e63-7631272ab6e3");
BLEByteCharacteristic   wifiConfigState("00000001-dabd-4a32-8e63-7631272ab6e3", BLERead | BLEWrite | BLEIndicate);
BLEStringCharacteristic wifiConfigSsid("00000002-dabd-4a32-8e63-7631272ab6e3", BLERead | BLEWrite | BLEIndicate, 32);
BLEStringCharacteristic wifiConfigPassword("00000003-dabd-4a32-8e63-7631272ab6e3", BLERead | BLEWrite | BLEIndicate, 16);

SensorResults::DHT11Results * readDHT11Sensor() 
{
	SensorResults::DHT11Results * const dht11Results = (SensorResults::DHT11Results *) malloc(sizeof(SensorResults::DHT11Results));  // Allocate address for results struct

	dht11Results->humidity    = humidityTemperatureSensor->readHumidity();
	dht11Results->temperature = humidityTemperatureSensor->readTemperature(true);

	return dht11Results;
}

SensorResults::SeesawResults * readSeesawSensor()
{
	SensorResults::SeesawResults * const seesawResults = (SensorResults::SeesawResults *) malloc(sizeof(SensorResults::SeesawResults));

	seesawResults->temperature = seesawSensor->getTemp();
	seesawResults->capacitance = seesawSensor->touchRead(0);

	return seesawResults;
}

SensorResults::LuxResults * readLuxSensor()
{
	SensorResults::LuxResults * const luxResults = (SensorResults::LuxResults *) malloc(sizeof(SensorResults::LuxResults));

	luxResults->luminescense = luxSensor->readLux();
	luxResults->white = luxSensor->readWhite();
	luxResults->ambientLight = luxSensor->readALS();

	return luxResults;
}

void getNetworks()
{
	Serial.println("Scanning networks");
	int numSsid = WiFi.scanNetworks();

	if (numSsid == -1) {
		Serial.println("Couldn't get WiFi connection");
		// TODO: raise error
	}

	Serial.println("Number of available networks: " + String(numSsid));

	for (size_t i = 0; i < numSsid; i++)
	{
		Serial.println("Network " + String(i) + ": " + String(WiFi.SSID(i)));
		Serial.println("Signal: " + String(WiFi.RSSI(i)) + " dBm");
		Serial.println("Encryption: " + DeviceCommissioning::encryptionTypeMap[WiFi.encryptionType(i)]);
		DeviceCommissioning::Network network = {
			WiFi.RSSI(i),
			WiFi.encryptionType(i),
			(char *) WiFi.SSID(i)
		};
	}
}

void scanner()
{
	if (!wifiScannerScanState.written()) {
		return;
	}

	Serial.println("Written" + String(wifiScannerScanState.value()));
	switch (wifiScannerScanState.value())
	{
		case DeviceCommissioning::IDLE:
			Serial.println("IDLE");
			delay(1000);
			break;
		case DeviceCommissioning::SCAN: {
			Serial.println("Starting WiFi scan");
			startWifi();

			wifiScannerScanState.writeValue(DeviceCommissioning::SCANNING);

			DeviceCommissioning::NetworkList networkInfo = getNetworks();
			// restart ble and send values
			startBle();

			Serial.println("Found " + String(networkInfo.count) + " networks");

			// chunk networks into packets
			// DeviceCommissioning::NetworkPacket packets[5];
			DeviceCommissioning::NetworkPacket packet = {0, {}};
			for (size_t i = 0; i < networkInfo.count; i++) {
				if (packet.count >= DeviceCommissioning::MAX_PACKET_NETWORK_COUNT) {
					// TODO: send multiple packets
					Serial.println("Packet overflow, ignoring other networks");
					break;
				}
				Serial.println("Copying network " + String(i));
				// copy network into packet memory and increment count
				packet.networks[i] = networkInfo.networks[i];
				packet.count++;
			}

			Serial.println("Converting packet to bytes");
			Serial.println("Packet contains " + String(packet.count) + " networks");

			char * buffer = reinterpret_cast<char*>(&packet);
			// print out buffer for debugging
			for (size_t j = 0; j < sizeof(packet); j++) {
				Serial.print((uint8_t) buffer[j]);
				Serial.print(", ");
			}
			Serial.println("");
			Serial.println("size: " + String(sizeof(packet)));
			wifiScannerAPList.writeValue(buffer, sizeof(packet), true);
			wifiScannerPacketCount.writeValue(networkInfo.count);
			wifiScannerScanState.writeValue(DeviceCommissioning::SCANNED);
			delete[] networkInfo.networks;
			Serial.println("Scan complete");
			break;
		}
		default:
			break;
	}
}

FlashStorage(flashStorage, DeviceCommissioning::PersistentInfo);                                                                         // This stupid fucking thing is a macro and `flashStorage` is the variable declaration
DeviceCommissioning::PersistentInfo persistentInfo;

inline void commission() 
{
	DeviceCommissioning::startBle();

	persistentInfo = flashStorage.read();
	if (persistentInfo.ssid != NULL && persistentInfo.password != NULL) {
		DeviceCommissioning::startWifi();
	}
	else {
		// TODO: Get from app
	}

	char * const ssid = persistentInfo.ssid == NULL ? WIFI_SSID : persistentInfo.ssid;
	char * const password = persistentInfo.password == NULL ? WIFI_PASSWORD : persistentInfo.password;

	while (status != WL_CONNECTED) {
		Serial.println("Attempting to connect to network");
		status = WiFi.begin(ssid, password);
		delay(5000);
	}

	Serial.println("Connected to network");

	persistentInfo.ssid = ssid;
	persistentInfo.password = password;
	persistentInfo.deviceId = 1;
	persistentInfo.encryptionType = ENC_TYPE_CCMP;

	flashStorage.write(persistentInfo);
}

inline void addCharacteristics();
{
	wifiScannerService.addCharacteristic(wifiScannerScanState);
	wifiScannerService.addCharacteristic(wifiScannerAPList);
	wifiScannerService.addCharacteristic(wifiScannerPacketCount);
	wifiScannerService.addCharacteristic(wifiScannerScanState);
	wifiConfiguratorService.addCharacteristic(wifiConfigState);
	batteryService.addCharacteristic(batteryPercentage);
	
	deviceInformationService.addCharacteristic(deviceManufacturerName);
	deviceInformationService.addCharacteristic(deviceModelNumber);
	deviceInformationService.addCharacteristic(deviceSerialNumber);
	deviceInformationService.addCharacteristic(deviceHardwareRevision);
	deviceInformationService.addCharacteristic(deviceFirmwareRevision);

}

void setup() 
{
	Serial.begin(SERIAL_BAUD_RATE);

	// #region Sensor Setup

	Wire.begin();

	while (!Serial);                                                                                                                  // Wait until the serial device responds
	Serial.println("Setting up sensors");

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

	// #endregion

	commission();

	addCharacteristics();
}

void loop() {
	SensorResults::DHT11Results  * const dht11Results  = readDHT11Sensor();
	SensorResults::SeesawResults * const seesawResults = readSeesawSensor();
	SensorResults::LuxResults    * const luxResults    = readLuxSensor();

	int8_t   const temperature  = dht11Results->temperature;
	uint8_t  const humidity     = dht11Results->humidity;
	uint16_t const capacitance  = seesawResults->capacitance;
	float    const luminescense = luxResults->luminescense;

	Serial.println("Temperature: " + String(temperature) + DEGREE_SYMBOL + "F");
	Serial.println("Humidity: " + String(humidity) + "%");
	Serial.println("Capacitance: " + String(capacitance) + "/1024");
	Serial.println("Luminescence: " + String(luminescense) + " lux\n");

	central = BLE.central();

	if (central) {
		while (central.connected()) {
			scanner();

			digitalWrite(BLUE_LED_PIN, HIGH);

			StaticJsonDocument<200> document;
			document["water"]       = capacitance;
			document["light"]       = luminescense;
			document["humidity"]    = humidity;
			document["temperature"] = temperature;
			document["time"]        = WiFi.getTime();

			serializeJson(document, postData);
			client->post("/post", CONTENT_TYPE, postData);

			status = client->responseStatusCode();
			Serial.print("Status Code: ");
			Serial.println(status);
			Serial.println(client->responseBody());
		}
	}

	delay(DELAY_RATE);
}