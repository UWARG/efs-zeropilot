#include "SBUS.hpp"
#if 0
SBUSPort::SBUSPort(UARTPortNum num, SBUSSettings settings) {
	UARTSettings port_settings;
	port_settings.rx_inverted = true;
	port_settings.timeout = 50;
	port_settings.stop_bits = 1;
	port_settings.cts_rts = false;
	port_settings.parity = UART_NO_PARITY; //double check this?
	port_settings.baudrate = 100000;

	port = UARTPort(num, port_settings);

	this->settings = settings;
}

StatusCode SBUSPort::setup() {
	if (is_setup) return STATUS_CODE_INVALID_ARGS;

	StatusCode status = port.setup();
	if (status != STATUS_CODE_OK) return status;

	status = port.setupDMA(0, 24); //setup DMA to be the largest size of a SBUS packet
	if (status != STATUS_CODE_OK) return status;

	is_setup = true;
	return STATUS_CODE_UNIMPLEMENTED;
}

StatusCode SBUSPort::reset() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	port.reset();

	is_setup = false;
	return STATUS_CODE_UNIMPLEMENTED;
}

uint8_t SBUSPort::get(PWMChannelNum num) {
	return 0;
}

uint32_t SBUSPort::get_us(PWMChannelNum num) {
	return 0;
}

bool SBUSPort::is_disconnected(uint32_t sys_time) {
	return false;
}

#endif
