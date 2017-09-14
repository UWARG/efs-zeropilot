#include "eeprom.h"

static const uint16_t EEPROM_ADDR = 0b1010000 << 1; // last 4 bits are E2, E1, E0

void EEPROM_Init(void) {
    HAL_GPIO_WritePin(MEM_WC_GPIO_Port, MEM_WC_Pin, GPIO_PIN_SET); // drive WC pin low to enable writing
}

void EEPROM_Write(uint16_t memAddress, uint8_t * data, uint16_t dataLen) {
    HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDR, memAddress, I2C_MEMADD_SIZE_16BIT, data, dataLen);
}

void EEPROM_Read(uint16_t memAddress, uint8_t *data, uint16_t dataLen) {
    HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDR, memAddress, I2C_MEMADD_SIZE_16BIT, data, dataLen);
}
