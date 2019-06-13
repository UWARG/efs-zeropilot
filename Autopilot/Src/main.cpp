#include "Clock.hpp"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"
#include "debug.h"
#include "eeprom.h"
#include <stdio.h>

extern "C" void MX_FREERTOS_Init(void);

int main(void)
{

  HAL_Init();

  initialize_system_clock();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI5_Init();
  MX_SPI4_Init();
  MX_I2C1_Init();
  MX_I2C4_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_ADC3_Init();
  MX_ADC2_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
//  MX_WWDG_Init();
  MX_CRC_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  debug("\n\nStarting up...");
  debug("Compiled on %s at %s", __DATE__, __TIME__);

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

  EEPROM_Init();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
//  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint64_t time;
  char buffer[100];
  while (1)
  {
    time = get_system_time_us();
    debug("Sys Time: %lu", (uint32_t)time);

    delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  debug("ERROR: [%s] : %d", __FILE__, __LINE__);
  while(1);
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */