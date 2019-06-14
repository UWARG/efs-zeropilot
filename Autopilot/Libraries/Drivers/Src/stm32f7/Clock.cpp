#include "Clock.hpp"
#include "Status.hpp"
#include "stm32f7xx_hal.h"
#include "DMA.hpp"

extern StatusCode get_status_code(HAL_StatusTypeDef status);

//for dma idle line detection with uart
extern DMAConfig uart2_dma_config;
extern DMAConfig uart3_dma_config;
extern DMAConfig uart4_dma_config;

StatusCode initialize_system_clock() {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0}};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0, 0, 0, 0, 0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
      {0, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  /**Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /**Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;

  StatusCode status = get_status_code(HAL_RCC_OscConfig(&RCC_OscInitStruct));
  if (status != STATUS_CODE_OK) return status;

  /**Activate the Over-Drive mode
  */
  status = get_status_code(HAL_PWREx_EnableOverDrive());
  if (status != STATUS_CODE_OK) return status;

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  status = get_status_code(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7));
  if (status != STATUS_CODE_OK) return status;

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2
      | RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_UART4
      | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_I2C2
      | RCC_PERIPHCLK_I2C4;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;

  status = get_status_code(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct));
  if (status != STATUS_CODE_OK) return status;

  return STATUS_CODE_OK;
}

uint32_t get_lsi_clock() {
  return 32000UL; //32khz lsi clock on stm32f7
}

uint64_t get_system_time_us() {
  return (uint64_t) (HAL_GetTick() * 1000ULL + TIM4->CNT / (get_system_clock() / 1000000UL));
}

/**
 *  Since the SysTick handler is overwridden by FreeRTOS, we use Timer4 as our system time
 *  This callback is the equivalent of the systick callback
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4) {
    HAL_IncTick();

    if (uart2_dma_config.timer == 1) {
      //call uart2 transfer complete with idle flag on
      uart2_dma_config.idle_line = 1;

      DMA_HandleTypeDef *dma_handle = (DMA_HandleTypeDef *) uart2_dma_config.dma_handle;
      dma_handle->XferCpltCallback(dma_handle);
    }

    if (uart2_dma_config.timer) uart2_dma_config.timer--;
  }
}