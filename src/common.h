#pragma once
#include <Arduino.h>

static const unsigned int BLE_MAX_CHARACTERISTIC_SIZE = 512;

uint8_t const RED_LED           = 2;
uint8_t const GREEN_LED         = 3;
uint8_t const BLUE_LED          = 4;
String  const MODEL             = "Flourish Device";
String  const SERIAL_NUMBER     = "abcde";
String  const HARDWARE_REVISION = "0.1";
String  const FIRMWARE_REVISION = "0.1";
char    const DEGREE_SYMBOL     = 248;

enum class FLOURISH_EXCEPTION {
	NO_WIFI_SSID
};

namespace DEVICE_STATE {
	enum {
		IDLE          = 1,
		COMMISSIONING = 2,
		COMMISSIONED  = 4,
		ERROR         = 8
	};
}
