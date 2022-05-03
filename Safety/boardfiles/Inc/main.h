/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define REC_UART_TX_Pin GPIO_PIN_2
#define REC_UART_TX_GPIO_Port GPIOA
#define REC_UART_RX_Pin GPIO_PIN_3
#define REC_UART_RX_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_1
#define BUZZER_GPIO_Port GPIOB
#define PPM_Pin GPIO_PIN_14
#define PPM_GPIO_Port GPIOB
#define RSSI_Pin GPIO_PIN_15
#define RSSI_GPIO_Port GPIOB
#define PWM6_Pin GPIO_PIN_6
#define PWM6_GPIO_Port GPIOC

#define PWM10_Pin GPIO_PIN_7
#define PWM10_GPIO_Port GPIOC
#define PWM11_Pin GPIO_PIN_8
#define PWM11_GPIO_Port GPIOC
#define PWM12_Pin GPIO_PIN_9
#define PWM12_GPIO_Port GPIOC
#define PWM1_Pin GPIO_PIN_8
#define PWM1_GPIO_Port GPIOA
#define PWM2_Pin GPIO_PIN_9
#define PWM2_GPIO_Port GPIOA
#define PWM3_Pin GPIO_PIN_10
#define PWM3_GPIO_Port GPIOA
#define PWM4_Pin GPIO_PIN_11
#define PWM4_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_12
#define LED3_GPIO_Port GPIOC

#define GRABBER_Pin_9_M2A GPIO_PIN_11
#define GRABBER_M2A_Port GPIOB
#define GRABBER_PIN_5_M2B GPIO_PIN_10
#define GRABBER_M2B_Port GPIOB

#define CRANE_Pin_7_M1A GPIO_PIN_15
#define CRANE_M1A_Port GPIOA
#define CRANE_Pin_8_M1B GPIO_PIN_3
#define CRANE_M1B_Port GPIOB

#define DBG_UART_TX_Pin GPIO_PIN_6
#define DBG_UART_TX_GPIO_Port GPIOB
#define DBG_UART_RX_Pin GPIO_PIN_7
#define DBG_UART_RX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);
void MX_FREERTOS_Init(void);
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
