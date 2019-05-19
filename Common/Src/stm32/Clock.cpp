#include "Clock.hpp"
#include "Status.hpp"

#if STM32F030xC
#include "stm32f0xx_hal.h"
#elif STM32F7xx
#include "stm32f7xx_hal.h"
#endif

extern StatusCode get_status_code(HAL_StatusTypeDef status);

uint32_t get_system_clock() {
	return HAL_RCC_GetSysClockFreq();
}

uint32_t get_peripheral_clock_apb1() {
	return HAL_RCC_GetPCLK1Freq();
}

uint32_t get_peripheral_clock_apb2() {
	return get_peripheral_clock_apb1();
}

uint32_t get_system_time() {
	return HAL_GetTick();
}

uint64_t get_system_time_us() {
	return (uint64_t) (HAL_GetTick() * 1000ULL + SysTick->VAL / (get_system_clock() / 1000000UL));
}

void delay(uint32_t ms) {
	HAL_Delay(ms);
}