#include "Arduino.h"
#include "ArduinoBLE.h"
#include "ArduinoHttpClient.h"
#include "WiFiNINA.h"

#include "../common.h"
#include "wifi.h"
#include "ble.h"

namespace WifiOperations
{
  const char* const SERVER_NAME  = "httpbin.org";
  const char* const PATH     	   = "/anything/devices/";
  uint8_t   	const SERVER_PORT  = 80;
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

    Serial.println("Posting data to " + String( PATH ) + String(deviceInformation.deviceId) + "/data");
    client->post(String( PATH ) + String(deviceInformation.deviceId) + "/data", CONTENT_TYPE, data);

    response->status = client->responseStatusCode();
    response->body   = (char *) client->responseBody().c_str();

    return response;
  }
}

