#include "Clock.hpp"
#include "stm32f0xx_hal.h"

StatusCode get_status_code(HAL_StatusTypeDef status);

//TODO: Dynamically create these values
uint32_t get_system_clock() {
	return HAL_RCC_GetSysClockFreq();
	//return 48000000U;
}

uint32_t get_peripheral_clock_apb1() {
	return HAL_RCC_GetPCLK1Freq();
	//return 48000000U;
}

uint32_t get_peripheral_clock_apb2() {
	return get_peripheral_clock_apb1();
}

uint32_t get_internal_clock() {
	return 8000000U;
}

//code is generated from STM32CubeMX
//configures clock to use our 8Mhz external source, etc..
StatusCode initialize_system_clock() {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI
		| RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

	StatusCode code = get_status_code(HAL_RCC_OscConfig(&RCC_OscInitStruct));
	if (code != STATUS_CODE_OK) return code;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	code = get_status_code(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1));
	if (code != STATUS_CODE_OK) return code;

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;

	code = get_status_code(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit));
	if (code != STATUS_CODE_OK) return code;

	HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	return STATUS_CODE_OK;
}