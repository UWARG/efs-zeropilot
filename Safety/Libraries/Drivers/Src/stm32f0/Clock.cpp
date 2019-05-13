#include "Clock.hpp"
#include "stm32f0xx_hal.h"
#include "Status.hpp"

extern StatusCode get_status_code(HAL_StatusTypeDef status);

static volatile uint32_t sys_time = 0;

uint32_t get_system_clock() {
	return HAL_RCC_GetSysClockFreq();
}

uint32_t get_peripheral_clock_apb1() {
	return HAL_RCC_GetPCLK1Freq();
}

uint32_t get_peripheral_clock_apb2() {
	return get_peripheral_clock_apb1();
}

uint32_t get_system_time(){
	return sys_time;
}

void delay(uint32_t ms){
	HAL_Delay(ms);
}

//code is generated from STM32CubeMX
//configures clock to use our 8Mhz external source, etc..
StatusCode initialize_system_clock() {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Configure LSE Drive Capability
  	*/
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);
	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
		|RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

	StatusCode status = get_status_code(HAL_RCC_OscConfig(&RCC_OscInitStruct));
	if (status != STATUS_CODE_OK) return status;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	status = get_status_code(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1));
	if (status != STATUS_CODE_OK) return status;

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
		|RCC_PERIPHCLK_RTC;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

	status = get_status_code(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit));
	if (status != STATUS_CODE_OK) return status;

	HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);

	return STATUS_CODE_OK;
}

//systick interrupt routine. Increments so we can track of current system time
void HAL_SYSTICK_Callback(){
	sys_time++;
}