#include "stm32f0xx_hal.h"
#include "Status.hpp"
#include <stdio.h>

ResetState get_reset_cause(){
	ResetState state;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)){
		state = RESET_STATE_WWDG;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)){
		state = RESET_STATE_IWDG;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)){
		state = RESET_STATE_SOFTWARE;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)){
		state = RESET_STATE_PWR;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)){
		state = RESET_STATE_PIN;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)){
		state = RESET_STATE_LOW_PWR;
	} else if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST)){
		state = RESET_STATE_OPTION_BYTE_LOADER;
	}

	//clear the reset flags
	__HAL_RCC_CLEAR_RESET_FLAGS();

	return state;
}

void print_reset_state(char* buffer, ResetState state){
	int len = sprintf(buffer, "Reset Cause: ");
	switch (state){
		case RESET_STATE_WWDG:
			sprintf(&buffer[len], "RESET_STATE_WWDG");
			break;
		case RESET_STATE_IWDG:
			sprintf(&buffer[len], "RESET_STATE_IWDG");
			break;
		case RESET_STATE_SOFTWARE:
			sprintf(&buffer[len], "RESET_STATE_SOFTWARE");
			break;
		case RESET_STATE_PWR:
			sprintf(&buffer[len], "RESET_STATE_PWR");
			break;
		case RESET_STATE_PIN:
			sprintf(&buffer[len], "RESET_STATE_PIN");
			break;
		case RESET_STATE_LOW_PWR:
			sprintf(&buffer[len], "RESET_STATE_LOW_PWR");
			break;
		case RESET_STATE_OPTION_BYTE_LOADER:
			sprintf(&buffer[len], "RESET_STATE_OPTION_BYTE_LOADER");
			break;
		default:
			sprintf(&buffer[len], "INVALID_RESET_STATE");
			break;
	}
}

