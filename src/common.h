#pragma once

#include "Arduino.h"
#include "bluetooth_commissioner.h"

static uint16_t const BLE_MAX_CHARACTERISTIC_SIZE = 512;
static uint8_t const JSON_SIZE	                  = 200;

uint8_t     const RED_LED           			= 2;
uint8_t     const GREEN_LED         			= 3;
uint8_t     const BLUE_LED          			= 4;

uint8_t     const DIGITAL_PIN_7     			= 7;

uint16_t    const DELAY_RATE        			= 10000;
uint16_t    const SERIAL_RATE       			= 9600;

uint8_t     const SEESAW_I2C_ADDRESS			= 0x36;
uint8_t     const SEESAW_CAPACATIVE_PIN	  = 0;

const char* const MODEL             		  = "Flourish Device";
const char* const SERIAL_NUMBER     		  = "abcde";
const char* const HARDWARE_REVISION 		  = "0.1";
const char* const FIRMWARE_REVISION 		  = "0.1";

char        const DEGREE_SYMBOL     			= 248;

extern BluetoothCommissioner commissioner;
extern uint8_t deviceState;

struct DeviceInformation
{
	unsigned int deviceId;
	String       deviceToken;
	String       name;
};
extern DeviceInformation deviceInformation;

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


void setupDevice();