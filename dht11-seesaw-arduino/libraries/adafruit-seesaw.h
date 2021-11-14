/*
	Author      : David Bicer, Jr. -- dmjman
	File        : adafruit-seesaw.h
	Project     : Flourish
	Created On  : 2021-11-12
	Description : Simplified library declarations for Adafruit Seesaw I2C Capacitive Soil Moisture Sensor.
	              This is based on the library created by Adafruit (https://github.com/adafruit/Adafruit_Seesaw).
*/

#ifndef SEESAW_H
#define SEESAW_H

#include "Arduino.h"
#include "Adafruit_I2CDevice.h"
#include <Wire.h>

namespace Seesaw
{
	uint8_t const SEESAW_STATUS_BASE          = 0x00;
	uint8_t const INPUT_PULLDOWN              = 0x03;
	uint8_t const SEESAW_STATUS_SWRST         = 0x7F;
	uint8_t const SEESAW_STATUS_TEMP          = 0x04;
	uint8_t const SEESAW_TOUCH_BASE           = 0x0F;
	uint8_t const SEESAW_TOUCH_CHANNEL_OFFSET = 0x10;
	uint8_t const SEESAW_STATUS_HW_ID         = 0x01;
	uint8_t const SEESAW_GPIO_BASE            = 0x01;
	uint8_t const SEESAW_GPIO_DIRSET_BULK     = 0x02;
	uint8_t const SEESAW_GPIO_DIRCLR_BULK     = 0x03;
	uint8_t const SEESAW_GPIO_BULK            = 0x04;
	uint8_t const SEESAW_GPIO_PULLENSET       = 0x0B;
	uint8_t const SEESAW_GPIO_BULK_SET        = 0x05;
	uint8_t const SEESAW_GPIO_BULK_CLR        = 0x06;
	uint8_t const SEESAW_HW_ID_CODE_SAMD09    = 0x55;
	uint8_t const SEESAW_HW_ID_CODE_TINY8X7   = 0x87;

	struct SeesawResults 
	{
		int16_t capacitance;
		float   temperature;
	};

	class Seesaw
	{
	public:
		Seesaw(TwoWire * i2cBus = NULL);
		~Seesaw(void);

		bool                 begin(uint8_t const i2cAddress, int8_t const flow = -1, bool const reset = true);
		float                getTemperature(void);
		int16_t              getCapacitance(void);
	private:
		int8_t               _flow;
		Adafruit_I2CDevice * _i2cDevice;
		TwoWire              _wire,
		                   * _i2cBus;
		uint8_t              _hardwareType;

		bool                 _write(uint8_t register const high, uint8_t register const low, uint8_t * const buffer = NULL, uint8_t const numBytes = 0);
		bool                 _read(uint8_t register const high, uint8_t register const low, uint8_t * const buffer, uint8_t const numBytes, uint16_t const delay = 125);
		void                 _pinModeBulk(uint32_t const pins, uint8_t & mode);
		void                 _pinModeBulk(uint32_t const portAPins, uint32_t const portBPins, uint8_t & mode);
		void                 _pinModeBulkWrite(uint8_t * const commands, uint8_t const numBytes, uint8_t & mode);
		void                 _pinMode(uint8_t const pin, uint8_t mode);
		void                 _readFlowWait(void);
		bool                 _digitalRead(uint8_t const pin);
		uint32_t             _digitalReadBulk(uint32_t const pins, uint8_t const bufferOffset = 0);
	};
}

#endif
