#include "Arduino.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

#include "../common.h"
#include "./wifi.h"
#include "./ble.h"

IPAddress ip;
WiFiClient wifi;

const char* const SERVER_NAME  = "httpbin.org";
uint8_t   	const WIFI_PORT    = 80;
const char* const CONTENT_TYPE = "application/json";
const char* const POST_URI     = "/post";

HttpClient * const client = new HttpClient(wifi, SERVER_NAME, WIFI_PORT);

void WifiOperations::startWifi()
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
	delay(100);
	Serial.println("WiFi initialized");
}

WifiOperations::PostResponse * WifiOperations::postData(char * const data)
{
	WifiOperations::PostResponse * const response = (WifiOperations::PostResponse *) malloc(sizeof(WifiOperations::PostResponse));

	client->post(POST_URI, CONTENT_TYPE, data);

	response->status = client->responseStatusCode();
	response->body   = (char *) client->responseBody().c_str();

	return response;
}