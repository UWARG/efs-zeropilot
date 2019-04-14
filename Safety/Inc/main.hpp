#pragma once

#include "GPIO.hpp"

const GPIOPort LED1_GPIO_PORT = GPIO_PORT_C;
const GPIOPinNum LED1_GPIO_PIN = 10;
const GPIOPort LED2_GPIO_PORT = GPIO_PORT_C;
const GPIOPinNum LED2_GPIO_PIN = 11;
const GPIOPort LED3_GPIO_PORT = GPIO_PORT_C;
const GPIOPinNum LED3_GPIO_PIN = 12;
const GPIOPort BUZZER_GPIO_PORT = GPIO_PORT_B;
const GPIOPinNum BUZZER_GPIO_PIN = 13;

#define REC_UART_TX_Pin GPIO_PIN_2
#define REC_UART_TX_GPIO_Port GPIOA
#define REC_UART_RX_Pin GPIO_PIN_3
#define REC_UART_RX_GPIO_Port GPIOA
#define PWM1_Pin GPIO_PIN_6
#define PWM1_GPIO_Port GPIOA
#define PWM2_Pin GPIO_PIN_7
#define PWM2_GPIO_Port GPIOA
#define PPM_Pin GPIO_PIN_1
#define PPM_GPIO_Port GPIOB
#define PWM3_Pin GPIO_PIN_14
#define PWM3_GPIO_Port GPIOB
#define PWM4_Pin GPIO_PIN_15
#define PWM4_GPIO_Port GPIOB
#define PWM5_Pin GPIO_PIN_6
#define PWM5_GPIO_Port GPIOC
#define PWM6_Pin GPIO_PIN_7
#define PWM6_GPIO_Port GPIOC
#define PWM7_Pin GPIO_PIN_8
#define PWM7_GPIO_Port GPIOC
#define PWM8_Pin GPIO_PIN_9
#define PWM8_GPIO_Port GPIOC
#define PWM9_Pin GPIO_PIN_8
#define PWM9_GPIO_Port GPIOA
#define PWM10_Pin GPIO_PIN_9
#define PWM10_GPIO_Port GPIOA
#define PWM11_Pin GPIO_PIN_10
#define PWM11_GPIO_Port GPIOA
#define PWM12_Pin GPIO_PIN_11
#define PWM12_GPIO_Port GPIOA

#define DBG_UART_TX_Pin GPIO_PIN_6
#define DBG_UART_TX_GPIO_Port GPIOB
#define DBG_UART_RX_Pin GPIO_PIN_7
#define DBG_UART_RX_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif