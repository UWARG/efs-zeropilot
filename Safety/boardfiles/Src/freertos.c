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
  * Copyright (c) 2022 STMicroelectronics International N.V.
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "main.h"
#include "stm32f0xx_hal_gpio.h"

#include "sensorFusionInterface.hpp"
#include "attitudeManagerInterface.h"
#include "RSSI.hpp"
//TODO: include RSSI Interface

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId attitudeManagerHandle;
osThreadId sensorFusionHandle;
osThreadId pidHandle;

/* USER CODE BEGIN Variables */
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void attitudeManagerExecute(void const * argument);
void sensorFusionExecute(void const * argument);
void pidExecute(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

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
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
  while (1) {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    for (int i = 0; i < 100000; i++) { }
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
    for (int i = 0; i < 900000; i++) { }
  }

}
/* USER CODE END 4 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

    SensorFusionInterfaceInit();
    AttitudeManagerInterfaceInit();
    pidInterfaceInit();
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

  /* Create the thread(s) */
  /* definition and creation of attitudeManager */
  osThreadDef(attitudeManager, attitudeManagerExecute, osPriorityNormal, 0, 256);
  attitudeManagerHandle = osThreadCreate(osThread(attitudeManager), NULL);

  /* definition and creation of sensorFusion */
  osThreadDef(sensorFusion, sensorFusionExecute, osPriorityNormal, 0, 2000);
  sensorFusionHandle = osThreadCreate(osThread(sensorFusion), NULL);

    /* definition and creation of pid */
  osThreadDef(pid, pidExecute, osPriorityNormal, 0, 2000);
  pidHandle = osThreadCreate(osThread(pid), NULL);
  
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* attitudeManagerExecute function */
void attitudeManagerExecute(void const * argument)
{

  /* USER CODE BEGIN attitudeManagerExecute */
  UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
  /* Infinite loop */
  for(;;)
  {
    // TODO: Re-add RSSI_CHECK
    RSSI_Check();
    AttitudeManagerInterfaceExecute();
    // HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    vTaskDelay(20);
  }

  /* USER CODE END attitudeManagerExecute */
}

/* sensorFusionExecute function */
void sensorFusionExecute(void const * argument)
{
  /* USER CODE BEGIN sensorFusionExecute */
  TickType_t xLastWakeTime;
  const TickType_t xTimeIncrement = 20;
  /* Inspect our own high water mark on entering the task. */
  UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

  /* Infinite loop */
  for(;;)
  {
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    // TODO: Re-add RSSI_CHECK
    // RSSI_Check();
    SensorFusionInterfaceExecute();
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
  }
  /* USER CODE END sensorFusionExecute */
}

/* pidExecute function */
void pidExecute(void const * argument)
{
  /* USER CODE BEGIN pidExecute */
  UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
  /* Infinite loop */
  for(;;)
  {
    pidInterfaceExecute();
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    vTaskDelay(20);
  }
  /* USER CODE END pidExecute */
}
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
