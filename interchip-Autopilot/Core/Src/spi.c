/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi6;

/* SPI6 init function */
void MX_SPI6_Init(void)
{

  /* USER CODE BEGIN SPI6_Init 0 */

  /* USER CODE END SPI6_Init 0 */

  /* USER CODE BEGIN SPI6_Init 1 */

  /* USER CODE END SPI6_Init 1 */
  hspi6.Instance = SPI6;
  hspi6.Init.Mode = SPI_MODE_MASTER;
  hspi6.Init.Direction = SPI_DIRECTION_2LINES;
  hspi6.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi6.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi6.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi6.Init.NSS = SPI_NSS_SOFT;
  hspi6.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi6.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi6.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi6.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi6.Init.CRCPolynomial = 7;
  hspi6.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi6.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI6_Init 2 */

  /* USER CODE END SPI6_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI6)
  {
  /* USER CODE BEGIN SPI6_MspInit 0 */

  /* USER CODE END SPI6_MspInit 0 */
    /* SPI6 clock enable */
    __HAL_RCC_SPI6_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI6 GPIO Configuration
    PA5     ------> SPI6_SCK
    PA6     ------> SPI6_MISO
    PA7     ------> SPI6_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_SPI6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI6 interrupt Init */
    HAL_NVIC_SetPriority(SPI6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI6_IRQn);
  /* USER CODE BEGIN SPI6_MspInit 1 */

  /* USER CODE END SPI6_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI6)
  {
  /* USER CODE BEGIN SPI6_MspDeInit 0 */

  /* USER CODE END SPI6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI6_CLK_DISABLE();

    /**SPI6 GPIO Configuration
    PA5     ------> SPI6_SCK
    PA6     ------> SPI6_MISO
    PA7     ------> SPI6_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* SPI6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI6_IRQn);
  /* USER CODE BEGIN SPI6_MspDeInit 1 */

  /* USER CODE END SPI6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
