#include "spi.h"
#include "interchip_S.hpp"

constexpr uint8_t MANUAL_OVERRIDE_CHANNEL {4};

static volatile Interchip_Packet rxData;
static volatile Interchip_Packet txData;
static volatile bool dataNew;

// Get the pwm signal that has been recieved.
volatile int8_t *getPWM() {
    dataNew = false;

    // Cause the structs are volatile, I've gotta use this to set them equal.
    txData.safetyLevel = rxData.safetyLevel;
    for (int i = 0; i < 12; i++) {
        txData.PWM[i] = rxData.PWM[i];
    }

    // call CRC everytime PWM data is called.
    // generate rx Data CRC.
    rxData.crc = crc_calc_modbus(rxData.PWM, 12);
    // call CRC counter

    return rxData.PWM;
}

volatile int8_t *getCRC() {
    return rxData.crc;
}

// Get the safety level that is currently being sent out.
uint16_t getSafetyLevel() {
    return txData.safetyLevel;
}

// Set the safety level that is currently being sent out.
void setSafetyLevel(uint16_t level) {
    txData.safetyLevel = level;
}

// return whether the data is new.
bool isDataNew() {
    return dataNew;
}

// Starts interchip interrupt. Call this once, not in a loop, each interchip callback
// sets up another interrupt.
void interchipInit() {
    HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t * ) & txData, (uint8_t * ) & rxData, sizeof(Interchip_Packet));
}

// spi callback that starts up another interrupt.
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    // Start Listening after the previous exchange.
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t * ) & txData, (uint8_t * ) & rxData, sizeof(Interchip_Packet));
    dataNew = true;
}

uint16_t crc_calc_modbus(const uint8_t msgBuffer[], size_t len) {
    {
        uint8_t low_byte_crc = 0xFF;  /* low byte of CRC initialized */
        uint8_t high_byte_crc = 0xFF; /* high byte of CRC initialized */
        uint8_t uIndex = 0;           /* will index into CRC lookup table */
        while (len--)                 /* pass through message buffer */
        {
            uIndex = high_byte_crc ^ *msgBuffer++; /* calculate the CRC */
            high_byte_crc = low_byte_crc ^ auchCRCHi[uIndex];
            low_byte_crc = auchCRCLo[uIndex];
        }
        return (high_byte_crc << 8) | low_byte_crc;
    }
}