#include "I2C.hpp"
#include "Status.hpp"

I2CPort::I2CPort(I2CPortNum port_num, I2CSpeed speed, I2CAddress address){
	//we need to initialize these at the very least
	sda = GPIOPin(I2C1_SDA_PORT, I2C1_SDA_PIN_NUM, GPIO_ALT_OD, GPIO_STATE_LOW, GPIO_RES_PULLUP, GPIO_SPEED_HIGH, GPIO_AF1_I2C1);
	scl = GPIOPin(I2C1_SCL_PORT, I2C1_SCL_PIN_NUM, GPIO_ALT_OD, GPIO_STATE_LOW, GPIO_RES_PULLUP, GPIO_SPEED_HIGH, GPIO_AF1_I2C1);
}

StatusCode I2CPort::setup(){
	return STATUS_CODE_OK;
}

StatusCode I2CPort::reset() {
	return STATUS_CODE_ISTATUS_CODE_OKNVALID_ARGS;
}

StatusCode I2CSlavePort::setup() {
	return STATUS_CODE_OK;
}

StatusCode I2CSlavePort::read_bytes(uint8_t *rx_data, size_t rx_len){
	return STATUS_CODE_OK;
}

StatusCode I2CSlavePort::write_bytes(uint8_t *tx_data, size_t tx_len){
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::setup(){
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::read_bytes(I2CAddress addr, uint8_t *rx_data, size_t rx_len){
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::write_bytes(I2CAddress addr, uint8_t *tx_data, size_t tx_len){
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::read_register(I2CAddress addr, uint16_t register_address, size_t register_address_size, uint8_t *rx_data, size_t rx_len){
	return STATUS_CODE_OK;
}

StatusCode I2CMasterPort::write_register(I2CAddress addr, uint16_t register_address, size_t register_address_size, uint8_t *tx_data, size_t tx_len){
	return STATUS_CODE_OK;
}

