/**
 * Implements a synchronous GPIO driver. Calls the underlying stm32HAL, and should be used in place of it
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

#include <stdint.h>
#include "Status.hpp"

/**
 * Available GPIO ports. Not all may be valid for the system! Read the datasheet
 */
typedef enum GPIOPort {
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E, //does not exist on stm32f0!
	GPIO_PORT_F
} GPIOPort;

typedef uint8_t GPIOPinNum;

/**
 * Direction of the GPIO pin. Can be input, output, our open-drain (OD) output
 * Use the ALT modes if the GPIO pin is being used for another purpose, such as SPI, I2C, etc..
 */
typedef enum GPIOMode {
	GPIO_INPUT = 0,
	GPIO_OUTPUT, //output push-pull
	GPIO_OUTPUT_OD, //output open-drain
	GPIO_ALT_PP, //alternate push-pull
	GPIO_ALT_OD //alternate open-drain
} GPIOMode;

/**
 * State of the pin. Only valid if its an output. High or Low
 */
typedef enum GPIOState {
	GPIO_STATE_LOW = 0,
	GPIO_STATE_HIGH
} GPIOState;

/**
 * Internal pull-up/pull-down resistor state of the pin
 */
typedef enum GPIOResistorState {
	GPIO_RES_NONE = 0,
	GPIO_RES_PULLUP,
	GPIO_RES_PULLDOWN
} GPIOResistorState;

/**
 * Low speeds for regular GPIO functions. High speeds when they're used for interfaces like SPI, I2C, etc..
 */
typedef enum GPIOSpeed {
	GPIO_SPEED_SLOW = 1,
	GPIO_SPEED_MEDIUM,
	GPIO_SPEED_FAST
} GPIOSpeed;

/**
 * Initialization for the gpio driver. Call this once!
 * @return STATUS_CODE_INTERNAL_ERROR if something went wrong
 */
StatusCode gpio_init();

/**
 * GPIOPin class. Use this to manipulate pins. Only 1 instance of this
 * class should exist for a particular pin. This class is not thread-safe either! If you're manipulating
 * the same pin within multiple threads implement your own mutexes.
 */
class GPIOPin {
 public:
	GPIOPin() = default; //need to have default constructor for compilation. Don't use it

	/**
	 * @param alternate_function Theres no typedef for this. These are defined inside stm32f0xx_hal_gpio.h
	 */
	GPIOPin(GPIOPort port,
			GPIOPinNum pin,
			GPIOMode mode,
			GPIOState initial_state,
			GPIOResistorState resistor_state = GPIO_RES_NONE,
			GPIOSpeed speed = GPIO_SPEED_SLOW,
			uint8_t alternate_function = 0);
	/**
	 * Initializes the GPIO with the HAL appropriately
	 * @return
	 */
	StatusCode setup();

	/**
	 * Sets the state of the pin
	 * @param new_state
	 * @return
	 */
	StatusCode set_state(GPIOState new_state);

	/**
	 * Toggles the state of the pin
	 * @return
	 */
	StatusCode toggle_state();

	/**
	 * Get the current state of the pin
	 * @param state Passed in reference. State will be written to here
	 * @return
	 */
	StatusCode get_state(GPIOState &state);

	/**
	 * Resets the pin to its default state.
	 * @return
	 */
	StatusCode reset();

 private:
	GPIOState current_state;
	GPIOMode mode;
	GPIOPort port;
	GPIOPinNum num;
	GPIOResistorState resistor_state;
	GPIOSpeed speed;
	uint8_t alternate_function;
};