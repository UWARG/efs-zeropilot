#ifndef INTERCHIP_A_H
#define INTERCHIP_A_H

#include "stm32f7xx_hal.h"
#include "Interchip.h"
#include <stdint.h>

#define INTERCHIP_TRANSMIT_DELAY 5

#ifdef __cplusplus
extern "C" {
#endif

// Sets the NSS line to high.
void Interchip_Init();

// Called in a freertos thread, this initiates the exchange of data over SPI.
void Interchip_Run();

// Allows for PWM values to be set.
void Interchip_SetPWM(int index, int data);

// Gets the safety level that is being sent by the safety chip.
uint8_t Interchip_GetAutonomousLevel(void);

// Function for calculating our CRC value
uint16_t crc_calc_modbus(volatile int16_t msgBuffer[], size_t len);

/**
* Callback for exclusive use by the SPI ISR.
* Users of this module should never call this function.
* Any manipulation of this function by the higher level voids all guarantees made by this module.
* */
void InterchipTxRxCallback();

// Returns 1 if the data is new, else it return 0.
uint8_t InterchipIsDataNew();

#ifdef __cplusplus
}
#endif

#endif
