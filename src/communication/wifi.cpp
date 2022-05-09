#include "Arduino.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

#include "./wifi.h"
#include "./ble.h"
#include "../common.h"

namespace WifiOperations
{
	WiFiClient wifi;

	const char * const SERVER_NAME  = "3.83.190.154";
	const char * const API_VERSION  = "v1";
	const char * const PATH         = "/devices/";
	const char * const CONTENT_TYPE = "application/json";
	uint16_t     const SERVER_PORT  = 5000;
	HttpClient * const client       = new HttpClient(wifi, SERVER_NAME, SERVER_PORT);

	void startWifi()
	{
		// stop ble
		Serial.println("Stopping BLE");
		if (BLE.connected()) {
			BLE.stopAdvertise();
			BLE.disconnect();
		}
		BLE.end();

		Serial.println("Initializing WiFi");

		// start WiFi
		wiFiDrv.wifiDriverDeinit();
		wiFiDrv.wifiDriverInit();

		// gives driver time to startup
		// TODO: is there a better way to do this
		delay(DELAY_RATE);
		Serial.println("WiFi initialized");
	}

	PostResponse * postData(const char * const data)
	{
		PostResponse * const response = new PostResponse();

		Serial.println("Posting data to " + String(API_VERSION) + String( PATH ) + String(deviceInformation.deviceId) + "/data");
		client->post(String(API_VERSION) + String( PATH ) + String(deviceInformation.deviceId) + "/data", CONTENT_TYPE, data);

		response->status = client->responseStatusCode();
		response->body   = (char *) client->responseBody().c_str();

		return response;
	}
}

