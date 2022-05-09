#include "Arduino.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

#include "../common.h"
#include "wifi.h"
#include "ble.h"

namespace WifiOperations
{
  const char* const SERVER_NAME  = "3.83.190.154";
  const char* const API_VERSION  = "v1";
  const char* const PATH     	   = "/devices/";
  uint16_t   	const SERVER_PORT  = 5000;
  const char* const CONTENT_TYPE = "application/json";

  WiFiClient wifi;
  HttpClient * client = new HttpClient(wifi, SERVER_NAME, SERVER_PORT);

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
    delay(100);
    Serial.println("WiFi initialized");
  }

  PostResponse * postData(char * const data)
  {
    PostResponse* const response = new PostResponse();

    Serial.println("Posting data to " + String(API_VERSION) + String( PATH ) + String(deviceInformation.deviceId) + "/data");
    client->post(String(API_VERSION) + String( PATH ) + String(deviceInformation.deviceId) + "/data", CONTENT_TYPE, data);

    response->status = client->responseStatusCode();
    response->body   = (char *) client->responseBody().c_str();

    return response;
  }
}

