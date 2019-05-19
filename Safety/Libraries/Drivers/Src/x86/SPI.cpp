#include "../../../../Common/Inc/SPI.hpp"
#include "../../../../Common/Inc/Status.hpp"

SPIPort::SPIPort(SPISettings settings) {
	this->settings = settings;
}

StatusCode SPIPort::setup() {
	has_setup = true;
	return STATUS_CODE_OK;
}

int32_t SPIPort::add_slave(GPIOPin slave_pin) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	if (num_slaves == MAX_SPI_SLAVES) {
		return -1;
	}

	slaves[num_slaves] = slave_pin;
	num_slaves++;
	return num_slaves - 1;
}

StatusCode SPIPort::set_slave(uint32_t slave_num) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	if (num_slaves > 0 && slave_num < num_slaves) {
		slaves[curr_slave].set_state(GPIO_STATE_LOW);
		slaves[slave_num].set_state(GPIO_STATE_HIGH);
		curr_slave = slave_num;
	}
	return STATUS_CODE_INVALID_ARGS;
}

StatusCode SPIPort::exchange_data(uint8_t *tx_data, uint8_t *rx_data, size_t length) {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	return STATUS_CODE_OK;
}

StatusCode SPIPort::reset() {
	if (!has_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}
	miso.reset();
	mosi.reset();
	nss.reset();
	sck.reset();

	has_setup = false;
	return STATUS_CODE_OK;
}