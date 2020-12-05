/**
 * SBUS is a protocol alternative to PPM and PWM that uses UART as a means of communication. It uses inverted UART
 * with a baudrate of 100,000
 *
 * This is just a template, currently this is unimplemented
 * @author ***
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#if 0

#pragma once

#include <stdint.h>
#include "GPIO.hpp"
#include "UART.hpp"
#include "PWM.hpp"

typedef struct SBUSSettings {
	uint32_t dummy;
} SBUSSettings;

class SBUSPort {
 public:
	SBUSPort(UARTPortNum port, SBUSSettings settings);

	StatusCode setup();

	StatusCode reset();

	/**
	 * Returns a percent value that was most recently received from the PPM channel, as a percentage
	 * from 0-100
	 * @param num
	 * @return 0 if an invalid channel number was given
	 */
	uint8_t get(PWMChannelNum num);

	/**
	 * Same as above function, but returns the captured value in microseconds instead
	 * @param num
	 * @return 0 if an invalid channel number was given
	 */
	uint32_t get_us(PWMChannelNum num);

	/**
	 * Wether the channel has disconnected based on the timeout
	 * @param sys_time Current system time in ms
	 * @return
	 */
	bool is_disconnected(uint32_t sys_time);

 private:
	bool is_setup = false;
	UARTPort port;
	SBUSSettings settings;
};

#endif
