#include "main.hpp"
#include "debug.h"
#include "safety_control.h"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "UART.hpp"
#include "stm32f0xx_hal.h"

int main(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//set clock sources
	initialize_system_clock();

	gpio_init();

	UARTSettings settings = {
		9600,
		1,
		UART_NO_PARITY,
		false
	};

	UARTPort debug_port = UARTPort(UART_PORT1, settings);

	char string[] = "hello how are you this is a test \n";

	debug_port.transmit((uint8_t*) string, 16);

//  MX_SPI1_Init();
//  MX_TIM1_Init();
//  MX_TIM3_Init();
//  MX_TIM14_Init();
//  MX_TIM15_Init();
//  MX_TIM16_Init();
//  MX_TIM17_Init();
//  MX_USART1_UART_Init();
//  MX_USART2_UART_Init();
//  MX_IWDG_Init();

  /* USER CODE BEGIN 2 */

//  debug("\r\n\r\nStarting up...");
//  debug("Compiled on %s at %s", __DATE__, __TIME__);

  GPIOPin led1 = GPIOPin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
  GPIOPin led2 = GPIOPin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
  GPIOPin led3 = GPIOPin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);

  led1.setup();
  led2.setup();
  led3.setup();

  led1.set_state(GPIO_STATE_HIGH);

//  Safety_Init();
//  Safety_Run();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
  	debug_port.transmit((uint8_t*) string, 16);
  /* USER CODE BEGIN 3 */
  }
}