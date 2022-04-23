#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

char     * const CONTENT_TYPE     = "application/json";
char     * const SERVER_NAME      = "httpbin.org";
uint16_t   const PORT             = 80;
uint16_t   const SERIAL_BAUD_RATE = 115200;
char     * const WIFI_SSID        = "Fios-5cJfH";
char     * const WIFI_PASSWORD    = "vane29waft98fan";

String postData;
IPAddress ip;
WiFiClient wifi;

HttpClient              * const client   = new HttpClient(wifi, SERVER_NAME, PORT);
StaticJsonDocument<200> * const document = new StaticJsonDocument<200>();

int16_t status = WL_IDLE_STATUS;

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial);

	while (status != WL_CONNECTED) {
		Serial.println("Attempting to connect to network");
		status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		delay(5000);
	}

	Serial.println("Connected to network");
}

void loop()
{
	ip = WiFi.localIP();
	Serial.println("IP: " + String(ip));
	Serial.println("Signal strength (RSSI): " + String(WiFi.RSSI()));

	document["water"] = 40.3;
	document["light"] = 30000;
	document["humidity"] = 80;
	document["temperature"] = 70.2;
	document["time"] = WiFi.getTime();

	serializeJson(document, postData);
	client.post("/post", CONTENT_TYPE, postData);

	status = client.responseStatusCode();
	Serial.print("Status Code: ");
	Serial.println(status);
	Serial.println(client.responseBody());

	delay(5000);
}
