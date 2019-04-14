#include <stdint.h>
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f030xc.h"

#include "GPIO.hpp"
#include "Status.hpp"

// the following helper functions convert between our defined gpio structures and the ones required by the mcu HAL
static GPIO_TypeDef *get_mcu_port(GPIOPort port);
static uint32_t get_mcu_pin(GPIOPinNum num);
static uint32_t get_mcu_pin_mode(GPIOMode mode);
static uint32_t get_mcu_resistor_mode(GPIOResistorState res);
static uint32_t get_mcu_speed(GPIOSpeed speed);

StatusCode gpio_init() {
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	return STATUS_CODE_OK;
}

GPIOPin::GPIOPin(GPIOPort port,
				 GPIOPinNum pin,
				 GPIOMode mode,
				 GPIOState initial_state,
				 GPIOResistorState resistor_state,
				 GPIOSpeed speed,
				 uint8_t alternate_function) {

	uint32_t mcu_pin = get_mcu_pin(pin);
	GPIO_TypeDef *mcu_port = get_mcu_port(port);

	if (mcu_port == nullptr) { //invalid port for the system given
		return;
	}

	//set the pin to reset
	HAL_GPIO_WritePin(get_mcu_port(port), (uint16_t) mcu_pin, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = mcu_pin;
	GPIO_InitStruct.Mode = get_mcu_pin_mode(mode);
	GPIO_InitStruct.Pull = get_mcu_resistor_mode(resistor_state);
	GPIO_InitStruct.Speed = get_mcu_speed(speed);
	GPIO_InitStruct.Alternate = alternate_function;

	HAL_GPIO_Init(mcu_port, &GPIO_InitStruct);

	this->mode = mode;
	this->num = pin;
	this->port = port;
	this->set_state(initial_state);
}

StatusCode GPIOPin::get_state(GPIOState &state) {
	if (this->mode == GPIO_OUTPUT || this->mode == GPIO_OUTPUT_OD) {
		state = current_state;
		return STATUS_CODE_OK;
	} else if (this->mode == GPIO_INPUT) {
		auto mcu_pin = (uint16_t) get_mcu_pin(this->num);
		GPIO_TypeDef *mcu_port = get_mcu_port(this->port);

		current_state = (GPIOState) HAL_GPIO_ReadPin(mcu_port, mcu_pin);
		state = current_state;
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS; //reading state in alternate function mode doesn't make sense
}

StatusCode GPIOPin::set_state(GPIOState new_state) {
	if (this->mode == GPIO_OUTPUT || this->mode == GPIO_OUTPUT_OD) {
		auto mcu_pin = (uint16_t) get_mcu_pin(this->num);
		GPIO_TypeDef *mcu_port = get_mcu_port(this->port);

		HAL_GPIO_WritePin(mcu_port, mcu_pin, static_cast<GPIO_PinState>(new_state));
		current_state = new_state;
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS; //only makes sense if the pin is an output
}

StatusCode GPIOPin::toggle_state() {
	if (current_state == GPIO_STATE_LOW) {
		return this->set_state(GPIO_STATE_HIGH);
	}
	return this->set_state(GPIO_STATE_LOW);
}

StatusCode GPIOPin::reset_pin() {
	auto mcu_pin = (uint16_t) get_mcu_pin(this->num);
	GPIO_TypeDef *mcu_port = get_mcu_port(this->port);
	HAL_GPIO_DeInit(mcu_port, mcu_pin);
	return STATUS_CODE_OK;
}

static GPIO_TypeDef *get_mcu_port(GPIOPort port) {
	switch (port) {
		case GPIO_PORT_A: return GPIOA;
		case GPIO_PORT_B: return GPIOB;
		case GPIO_PORT_C: return GPIOC;
		case GPIO_PORT_D: return GPIOD;
		case GPIO_PORT_F: return GPIOF;
		default: return nullptr;
	}
}

static uint32_t get_mcu_pin(GPIOPinNum num) {
	return 1U << num;
}

static uint32_t get_mcu_pin_mode(GPIOMode mode) {
	switch (mode) {
		case GPIO_INPUT: return GPIO_MODE_INPUT;
		case GPIO_OUTPUT: return GPIO_MODE_OUTPUT_PP;
		case GPIO_OUTPUT_OD: return GPIO_MODE_OUTPUT_OD;
		case GPIO_ALT_OD: return GPIO_MODE_AF_OD;
		case GPIO_ALT_PP: return GPIO_MODE_AF_PP;
		default: return GPIO_MODE_INPUT;
	}
}

static uint32_t get_mcu_resistor_mode(GPIOResistorState res) {
	switch (res) {
		case GPIO_RES_NONE: return GPIO_NOPULL;
		case GPIO_RES_PULLDOWN: return GPIO_PULLDOWN;
		case GPIO_RES_PULLUP: return GPIO_PULLUP;
		default: return GPIO_NOPULL;
	}
}

static uint32_t get_mcu_speed(GPIOSpeed speed) {
	switch (speed) {
		case GPIO_SPEED_LOW: return GPIO_SPEED_FREQ_LOW;
		case GPIO_SPEED_MED: return GPIO_SPEED_FREQ_MEDIUM;
		case GPIO_SPEED_HIGH: return GPIO_SPEED_FREQ_HIGH;
		default: return GPIO_SPEED_FREQ_LOW;
	}
}