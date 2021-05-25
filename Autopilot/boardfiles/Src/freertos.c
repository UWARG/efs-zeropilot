/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AttitudeManagerInterface.h"
#include "PathManagerInterface.h"
#include "telemetryManagerInterface.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static const int PERIOD_ATTITUDEMANAGER_MS = 20;
static const int PERIOD_PATHMANAGER_MS = 10; //TO CONFIRM
static const int PERIOD_TELEMETRY_MS = 10; //TO CONFIRM

/* USER CODE END Variables */
osThreadId attitudeManagerHandle;
osThreadId InterchipHandle;
osThreadId pathManagerHandle;
osThreadId telemetryRunHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void attitudeManagerExecute(void const * argument);
extern void Interchip_Run(void const * argument);
void pathManagerExecute(void const * argument);
void StartTelemetryRun(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of attitudeManager */
  osThreadDef(attitudeManager, attitudeManagerExecute, osPriorityNormal, 0, 128);
  attitudeManagerHandle = osThreadCreate(osThread(attitudeManager), NULL);

  /* definition and creation of Interchip */
  osThreadDef(Interchip, Interchip_Run, osPriorityNormal, 0, 128);
  InterchipHandle = osThreadCreate(osThread(Interchip), NULL);

  /* definition and creation of pathManager */
  osThreadDef(pathManager, pathManagerExecute, osPriorityBelowNormal, 0, 128);
  pathManagerHandle = osThreadCreate(osThread(pathManager), NULL);

  /* definition and creation of telemetryRun */
  osThreadDef(telemetryRun, StartTelemetryRun, osPriorityBelowNormal, 0, 128);
  telemetryRunHandle = osThreadCreate(osThread(telemetryRun), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_attitudeManagerExecute */
/**
  * @brief  Function implementing the attitudeManager thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_attitudeManagerExecute */
void attitudeManagerExecute(void const * argument)
{
  /* USER CODE BEGIN attitudeManagerExecute */
  /* Infinite loop */
  while(1)
  {
    //TickType_t xLastWakeTime = xTaskGetTickCount();
    //vTaskDelayUntil(&xLastWakeTime, PERIOD_ATTITUDEMANAGER_MS);
    //AttitudeManagerInterfaceExecute();
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    //HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    //HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    osDelay(100);
    //HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  }
  /* USER CODE END attitudeManagerExecute */
}

/* USER CODE BEGIN Header_pathManagerExecute */
/**
* @brief Function implementing the pathManager thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pathManagerExecute */
void pathManagerExecute(void const * argument)
{
  /* USER CODE BEGIN pathManagerExecute */
  /* Infinite loop */
  while(1)
  {
    //TickType_t xLastWakeTime = xTaskGetTickCount();
    //vTaskDelayUntil(&xLastWakeTime, PERIOD_PATHMANAGER_MS);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    osDelay(200);
    //PathManagerInterfaceExecute();
  }
  
  /* USER CODE END pathManagerExecute */
}

/* USER CODE BEGIN Header_StartTelemetryRun */
/**
* @brief Function implementing the telemetryRun thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTelemetryRun */
void StartTelemetryRun(void const * argument)
{
  /* USER CODE BEGIN StartTelemetryRun */
  /* Infinite loop */
  while(1)
  {
    //TickType_t xLastWakeTime = xTaskGetTickCount();
    //vTaskDelayUntil(&xLastWakeTime, PERIOD_TELEMETRY_MS);
    //TelemetryManagerInterfaceExecute();
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    osDelay(300);
  }
  
  /* USER CODE END StartTelemetryRun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
