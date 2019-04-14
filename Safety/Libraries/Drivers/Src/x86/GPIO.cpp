#include "GPIO.hpp"
#include "Status.hpp"
#include <stdint.h>

StatusCode gpio_init() {
	return STATUS_CODE_OK;
}

GPIOPin::GPIOPin(GPIOPort port,
				 GPIOPinNum pin,
				 GPIOMode mode,
				 GPIOState initial_state,
				 GPIOResistorState resistor_state,
				 GPIOSpeed speed,
				 uint8_t alternate_function) {
	this->mode = mode;
	this->num = num;
	this->port = port;
	this->set_state(initial_state);
}

StatusCode GPIOPin::get_state(GPIOState &state) {
	if (this->mode == GPIO_OUTPUT_OD || this->mode == GPIO_OUTPUT) {
		state = this->current_state;
		return STATUS_CODE_OK;
	} else if (this->mode == GPIO_INPUT) {
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS; //reading state in alternate function mode doesn't make sense
}

StatusCode GPIOPin::set_state(GpioState new_state) {
	if (this->mode == GPIO_OUTPUT_OD || this->mode == GPIO_OUTPUT) {
		current_state = new_state;
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS; //only makes sense if the pin is an output
}

StatusCode GPIOPin::toggle_state() {
	return this->set_state(!current_state);
}

StatusCode GPIOPin::reset_pin() {
	return STATUS_CODE_OK;
}

