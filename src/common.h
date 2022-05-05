#pragma once

#include "Arduino.h"

static uint16_t const BLE_MAX_CHARACTERISTIC_SIZE = 512;

uint8_t    const RED_LED           = 2;
uint8_t    const GREEN_LED         = 3;
uint8_t    const BLUE_LED          = 4;
uint8_t    const DIGITAL_PIN_7     = 7;
uint16_t   const DELAY_RATE        = 1000;
uint16_t   const SERIAL_RATE       = 9600;
char     * const MODEL             = "Flourish Device";
char     * const SERIAL_NUMBER     = "abcde";
char     * const HARDWARE_REVISION = "0.1";
char     * const FIRMWARE_REVISION = "0.1";
char       const DEGREE_SYMBOL     = 248;

enum class FLOURISH_EXCEPTION {
	NO_WIFI_SSID
};

namespace DEVICE_STATE 
{
	enum {
		IDLE          = 1,
		COMMISSIONING = 2,
		COMMISSIONED  = 4,
		ERROR         = 8
	};
}
