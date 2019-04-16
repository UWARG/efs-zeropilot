#pragma once

#include <stdint.h>

/**
 * Retrieves the system clock rate
 * @return Frequency in Mhz
 */
uint32_t get_system_clock();

/**
 * Returns the peripheral clock for the APB bus. This will vary depending on the chip.
 * Depending on the chip it will also control different interfaces. Ie. APB1 peripheral
 * clock may be used for UART1, I2C1, SPI1 etc.. APB2 may be used for SPI2, UART3, etc..
 * @return Frequency in Mhz
 */
uint32_t get_peripheral_clock_apb1();

/**
 * If apb2 exists on chip, returns the clock rate. Otherwise returns same
 * value as apb1. Only the stm32f7 has the seperate APB2 high speed bus
 * @return Frequency in Mhz
 */
uint32_t get_peripheral_clock_apb2();