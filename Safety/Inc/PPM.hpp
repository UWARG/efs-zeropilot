/**
 * Implements PPM driver. Support variable PPM, where the input can be
 * from 8-12 channels. Note that the assumption is that we're going to be
 * reading signals that range from 1-2ms. If you're trying to read outside of this range,
 * modify the prescaler in the implementation, as we may get a timer overflow or get really bad precision!
 * Its fine if the signals we're reading are around the same ranges however, like 800us to 2200us. In that case
 * just modify the setLimits() so the percentages received are correct
 * @author Serj Babayan, Anthony Berbari
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 *
 *  Hardware Info:
 *
 *  Timer14 - PPM
 *  PPM PIN - B1
 */
#pragma once

#include "PWM.hpp"
#include <stdint.h>

static const int32_t MAX_PPM_CHANNELS = 12;

enum StatusCode{STATUS_CODE_OK, STATUS_CODE_FAILED, STATUS_CODE_INVALID_ARGS};

class PPMChannel {
 public:
	/**
	 * How many channels are we expecting in the input port?
	 * Usually this is only 8
	 * @param num_channels
	 * @param disconnect_timeout Number of ms to wait before we consider channel disconnected
	 */
	explicit PPMChannel(uint8_t num_channels = 8, uint32_t disconnect_timeout = 1000);

	/**
	 * Reconfigure number of channels
	 * @param num_channels
	 */
	StatusCode setNumChannels(uint8_t num_channels);

	/**
	 * Set expected input limits for a particular channel
	 * @param channel
	 * @param min Min time in us (for a 0% signal)
	 * @param max Max time in us (for 100% signal)
	 * @param deadzone time in us for deadzone. ie. if deadzone is set to 50, a signal that is received
	 * 		with a 1050us length will still be considered 0%
	 */
	StatusCode setLimits(uint8_t channel, uint32_t min, uint32_t max, uint32_t deadzone);

	// /**
	//  * Set the disconnect timeout
	//  * @param timeout
	//  * @return
	//  */
	// StatusCode setTimeout(uint32_t timeout);

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
 	PWMChannelNum num_channels = 0;
	float min_values[MAX_PPM_CHANNELS]; //stores min microsecond values for each channel
	float max_values[MAX_PPM_CHANNELS]; //stores max microsecond values for each channel
	uint32_t disconnect_timeout;
	bool is_setup = false;

//	uint32_t counter_to_time(uint16_t count, uint32_t psc);
//	uint8_t time_to_percentage(uint32_t max, uint32_t min, uint32_t deadzone, uint32_t time);

};
