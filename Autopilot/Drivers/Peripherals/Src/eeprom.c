#include "eeprom.h"
#include "cmsis_os.h"
#include "debug.h"

static const uint16_t EEPROM_ADDR = 0b1010000 << 1; // last 4 bits are E2, E1, E0

static TaskHandle_t currentTask = NULL;

void EEPROM_Init(void) {
    HAL_GPIO_WritePin(MEM_WC_GPIO_Port, MEM_WC_Pin, GPIO_PIN_SET); // drive WC pin
}

void EEPROM_Write(uint16_t memAddress, uint8_t * data, uint16_t dataLen) {
    HAL_GPIO_WritePin(MEM_WC_GPIO_Port, MEM_WC_Pin, GPIO_PIN_RESET); // drive WC pin low to enable writing
    HAL_StatusTypeDef err = HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDR, memAddress, I2C_MEMADD_SIZE_16BIT, data, dataLen);
    HAL_Delay(100);
    debug("w err: %02x", err);
    HAL_GPIO_WritePin(MEM_WC_GPIO_Port, MEM_WC_Pin, GPIO_PIN_SET); // return high
}

void EEPROM_Read(uint16_t memAddress, uint8_t *data, uint16_t dataLen) {
    // HAL_GPIO_WritePin(MEM_WC_GPIO_Port, MEM_WC_Pin, GPIO_PIN_SET); // drive WC pin
    HAL_StatusTypeDef err = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, memAddress, I2C_MEMADD_SIZE_16BIT, data, dataLen, 0xfff);
    // uint32_t noti = ulTaskNotifyTake(pdTRUE, 0xFFFF);
    // if (noti == 0) return;
    debug("r err: %02x", err);
}

// called from I2C Rx ISR
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    BaseType_t higherPrioTaskWoken = pdFALSE;
    if (hi2c->Instance == I2C1) {
        xTaskNotifyFromISR(currentTask, 0, eNoAction, &higherPrioTaskWoken);
        portYIELD_FROM_ISR(higherPrioTaskWoken)
    }
}

    