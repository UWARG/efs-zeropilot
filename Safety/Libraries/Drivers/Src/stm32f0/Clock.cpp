#include "Clock.hpp"
#include "stm32f0xx_hal_rcc.h"

//TODO: Dynamically create these values
uint32_t get_system_clock(){
	return HAL_RCC_GetSysClockFreq();
	//return 48000000U;
}

uint32_t get_peripheral_clock_apb1(){
	return HAL_RCC_GetPCLK1Freq();
	//return 48000000U;
}

uint32_t get_peripheral_clock_apb2(){
	return get_peripheral_clock_apb1();
}

uint32_t get_internal_clock(){
	return 8000000U;
}