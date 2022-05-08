#include "Arduino.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

#include "../common.h"
#include "wifi.h"
#include "ble.h"

namespace WifiOperations
{
	WiFiClient wifi;

	const char* const SERVER_NAME  = "httpbin.org";
	uint8_t   	const WIFI_PORT    = 80;
	const char* const CONTENT_TYPE = "application/json";
	const char* const POST_URI     = "/post";

	HttpClient * const client = new HttpClient(wifi, SERVER_NAME, WIFI_PORT);

	void startWifi()
	{
		// stop ble
		Serial.println("Stopping BLE");
		BLE.stopAdvertise();
		BLE.disconnect();
		BLE.end();

		Serial.println("Initializing WiFi");

		// start WiFi
		wiFiDrv.wifiDriverDeinit();
		wiFiDrv.wifiDriverInit();
		// gives driver time to startup
		// TODO: is there a better way to do this
		// delay(100);
		delay(500);
		Serial.println("WiFi initialized");
	}

	PostResponse * postData(char * const data)
	{
		PostResponse * const response = (PostResponse *) malloc(sizeof(PostResponse));

		Serial.println("Posting data");
		client->post(POST_URI, CONTENT_TYPE, data);

		response->status = client->responseStatusCode();
		response->body   = (char *) client->responseBody().c_str();

		return response;
	}
}

