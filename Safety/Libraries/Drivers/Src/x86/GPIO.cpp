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
}

StatusCode GPIOPin::setup() {
	return STATUS_CODE_UNIMPLEMENTED;
}

StatusCode GPIOPin::get_state(GPIOState &state) {
	return STATUS_CODE_UNIMPLEMENTED;
}

StatusCode GPIOPin::set_state(GpioState new_state) {
	return STATUS_CODE_UNIMPLEMENTED;
}

StatusCode GPIOPin::toggle_state() {
	return STATUS_CODE_UNIMPLEMENTED;
}

StatusCode GPIOPin::reset_pin() {
	return STATUS_CODE_UNIMPLEMENTED;
}

