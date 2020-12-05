/**
 * Implements abstracted PWM driver for the stm32F0. For pin mappings, look at the .cpp file
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 *
 * Implementation details for PWM generation on our STM32F0:
 *
 * What timer controls what PWM channels:
 *
 * TIM16_CH1 - PWM 1
 * TIM17_CH1 - PWM 2
 * TIM15_CH1-CH2 - PWM 3-4
 * TIM3_CH1-CH4 - PWM 5-8
 * TIM1_CH1-CH4 - PWM 9-12
 * Timer14 - PPM
 *
 * The above means we can only set the frequencies of a group of PWMs controlled by a single timer!
 * We can't individually control frequencies of channels!
 */

#pragma once

#include <stdint.h>

typedef uint8_t PWMChannelNum;

/**
 * Represents a group of PWM outputs that map to a single timer
 * Settings can be changed on a per-group basis
 */
typedef enum PWMGroup {
	PWM_GROUP_1,
	PWM_GROUP_2,
	PWM_GROUP_3_4,
	PWM_GROUP_5_8,
	PWM_GROUP_9_12
} PWMGroup;

typedef struct PWMGroupSetting {
	uint32_t period; //period of pulse in us
	uint32_t min_length; //min pulse length in us
	uint32_t max_length; //max pulse length in us
	bool inverted = false;
} PWMGroupSetting;

#if 0
class PWMChannel {
 public:
	PWMChannel() = default;
	PWMChannel(GPIOPort port, GPIOPinNum pin_num, uint8_t alternate_function, void *timer, uint16_t channel);

	/**
	 * Initializes the GPIO, timers, interrupts, etc..
	 * If already setup, will call reset() first
	 * @return
	 */
	StatusCode setup();

	/**
	 * Resets everything to its default state, including all the GPIOs
	 * @return
	 */
	StatusCode reset();

	/**
	 * Sets the min and max signals. Used for percentage conversion
	 * @param min
	 * @param max
	 */
	void setLimits(uint32_t min, uint32_t max);

	/**
	 * Set the output to a specific pulse width based on a percentage
	 * @param percent
	 */
	void set(uint8_t percent);

 private:
	GPIOPin pin;
	void *timer;
	uint16_t timer_channel;
	uint32_t min_signal = 1000; //standard for 50hz pwm signal
	uint32_t max_signal = 2000;
};

/**
 * Singleton class that should be used for managing the PWM interface
 * By default configures all the ports for 50Hz output. Call the configure()
 * function if you want something different. ie. 500Hz on some ESC, 50Hz on the servos, etc..
 */
class PWMManager {
 public:
	/**
	 * Gets an instance of the singleton, or initializes it
	 * @return
	 */
	static PWMManager &getInstance();

	/**
	 * Configure a PWM port with the appropriate frequency, etc..
	 * Call setup() afterwards
	 * @param group
	 * @param setting
	 * @return
	 */
	StatusCode configure(PWMGroup group, PWMGroupSetting setting);

	/**
	 * Initializes the GPIO, timers, interrupts, etc..
	 * If already setup, will call reset() first
	 * @return
	 */
	StatusCode setup();

	/**
	 * Resets everything to its default state, including all the GPIOs
	 * @return
	 */
	StatusCode reset();

	/**
	 * Returns a reference to the requested channel. Note that this is 1-indexed!
	 * @param num
	 * @return Returns the first channel if invalid channel num is given
	 */
	PWMChannel &channel(PWMChannelNum num);

	/**
	 * Sets all output channels
	 * @param percent
	 * @return
	 */
	StatusCode set_all(uint8_t percent);

	//disable copy and assignment constructors
	PWMManager(PWMManager const &) = delete;
	void operator=(PWMManager const &) = delete;

 private:
	PWMManager() = default; //force initialization through getInstance()
	PWMChannel channels[12];
	bool is_setup = false;
	uint16_t prescaler;
};

#endif
