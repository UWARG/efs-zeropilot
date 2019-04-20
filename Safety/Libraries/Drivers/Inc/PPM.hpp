/**
 * Implements PPM driver. Support variable PPM, where the input can be
 * from 8-12 channels.
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 *
 *  Hardware Info:
 *
 *  Timer14 - PPM
 *  PPM PIN - B1
 */

#include <stdint.h>
#include "GPIO.hpp"
#include "PWM.hpp"

static const int32_t MAX_PPM_CHANNELS = 12;

class PPMChannel {
 public:
	/**
	 * How many channels are we expecting in the input port?
	 * Usually this is only 8
	 * @param num_channels
	 */
	explicit PPMChannel(uint8_t num_channels = 8);

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
	 */
	StatusCode setLimits(uint8_t channel, uint32_t min, uint32_t max);

	/**
	 * Setup timer14 with interrupts, gpios, etc..
	 * @return
	 */
	StatusCode setup();

	/**
	 * Reset all pins to their default state. Stop the timer and interrupts
	 * @return
	 */
	StatusCode reset();

	/**
	 * Returns a percent value that was most recently received from the PPM channel
	 * @param num
	 * @return 0 if an invalid channel number was given
	 */
	uint8_t get(PWMChannelNum num);

	//static below since our interrupt handler needs access to these
	//don't access these!
	static uint8_t num_channels;
	static volatile uint16_t capture_value[MAX_PPM_CHANNELS]; //support up to 12 channels
	static volatile uint8_t ppm_index;

 private:
	int32_t min_values[MAX_PPM_CHANNELS]; //stores min tick values for each channel
	int32_t max_values[MAX_PPM_CHANNELS]; //stores max tick values for each channel
	bool is_setup = false;
	GPIOPin ppm_pin;
};