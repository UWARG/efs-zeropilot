/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

#include "DMA.hpp"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim14;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

extern DMAConfig i2c1_dma_config;
extern DMAConfig uart2_dma_config;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void) {
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */

	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void) {
	/* USER CODE BEGIN HardFault_IRQn 0 */
	__asm("BKPT #0\n"); // Break into the debugger
	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {
	/* USER CODE BEGIN SVC_IRQn 0 */

	/* USER CODE END SVC_IRQn 0 */
	/* USER CODE BEGIN SVC_IRQn 1 */

	/* USER CODE END SVC_IRQn 1 */
}

//void Reset_Handler(void){
//
////	while (1)
////	{
////		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
////		/* USER CODE END W1_HardFault_IRQn 0 */
////	}
//}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void) {
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	if (uart2_dma_config.timer == 1) {
		//call uart2 transfer complete with idle flag on
		uart2_dma_config.idle_line = 1;

		DMA_HandleTypeDef *dma_handle = (DMA_HandleTypeDef *) uart2_dma_config.dma_handle;
		dma_handle->XferCpltCallback(dma_handle);
	}

	if (uart2_dma_config.timer) uart2_dma_config.timer--;

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 2 and 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void) {
	/* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

	/* USER CODE END DMA1_Channel2_3_IRQn 0 */
	//HAL_DMA_IRQHandler(&hdma_spi1_rx);
	HAL_DMA_IRQHandler((DMA_HandleTypeDef *) i2c1_dma_config.dma_handle);
	/* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

	/* USER CODE END DMA1_Channel2_3_IRQn 1 */
}
//
///**
//  * @brief This function handles DMA1 channel 4 and 5 interrupts.
//  */
void DMA1_Channel4_5_IRQHandler(void) {
	/* USER CODE BEGIN DMA1_Channel4_5_IRQn 0 */

	/* USER CODE END DMA1_Channel4_5_IRQn 0 */
	HAL_DMA_IRQHandler((DMA_HandleTypeDef *) uart2_dma_config.dma_handle);
	/* USER CODE BEGIN DMA1_Channel4_5_IRQn 1 */

	/* USER CODE END DMA1_Channel4_5_IRQn 1 */
}

///**
//  * @brief This function handles TIM14 global interrupt.
//  */
void TIM14_IRQHandler(void) {
	/* USER CODE BEGIN TIM14_IRQn 0 */

	/* USER CODE END TIM14_IRQn 0 */
	HAL_TIM_IRQHandler(&htim14);
	/* USER CODE BEGIN TIM14_IRQn 1 */

	/* USER CODE END TIM14_IRQn 1 */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void) {
	/* USER CODE BEGIN SPI1_IRQn 0 */

	/* USER CODE END SPI1_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi1);
	/* USER CODE BEGIN SPI1_IRQn 1 */

	/* USER CODE END SPI1_IRQn 1 */
}

void USART2_IRQHandler(void) {
	/* UART IDLE Interrupt */
	if ((USART2->ISR & USART_ISR_IDLE) != RESET) {
		USART2->ICR = UART_CLEAR_IDLEF;
		/* Start DMA timer */
		uart2_dma_config.timer = uart2_dma_config.timeout;
	}

	HAL_UART_IRQHandler(&huart2);
}

void I2C1_IRQHandler(void) {
	if ((I2C1->ISR & I2C_ISR_TCR) != RESET) { //if we got stop bit complete callback
		I2C1->ICR = I2C_ICR_STOPCF;

		i2c1_dma_config.idle_line = 1;

		DMA_HandleTypeDef *dma_handle = (DMA_HandleTypeDef *) i2c1_dma_config.dma_handle;
		dma_handle->XferCpltCallback(dma_handle); //call our transfer complete callback
	}

	HAL_I2C_EV_IRQHandler(&hi2c1);
	HAL_I2C_ER_IRQHandler(&hi2c1);
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
