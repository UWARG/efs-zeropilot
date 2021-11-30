/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "mpu6050_driver.hpp"
#include "stm32f0xx_hal.h"








/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


#define WHO_AM_I 0x75 // Use to check if device is there

#define IMU_ADDRESS 0x68 <<1 // 7-bit address

#define PWR_MGMT_1 0x6B

#define SMPLRT_DIV 0x19

//Config
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

//Gryo registers
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x44
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

//Accelerometer register
#define ACCEL_XOUT_H 0x3B

// constants

const uint8_t kImuAddr = 104;
const double kGryoCorrector = 131.0;
const double kAccelCorrector = 16384.0;




/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void imuInit ()
{
	//Use this for IMU set up
	uint8_t imu_check = 0, data_check = 0;
	HAL_StatusTypeDef wake = HAL_OK;


	//Check if device is present and the IMU is the expected device
	{
		if (HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, WHO_AM_I, 1, &imu_check, 1, HAL_MAX_DELAY) == HAL_OK && imu_check == kImuAddr){

				//Wake the sensor up
				wake = HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, PWR_MGMT_1, 1, &data_check, 1, HAL_MAX_DELAY);

				data_check = 0x07;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, SMPLRT_DIV, 1, &data_check, 1, HAL_MAX_DELAY);

				//Configure gyro to have range of +/- 250 dps
				data_check = 0x00;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, GYRO_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);

				//Configure Accelerometer to have range of +/- 2g
				data_check = 0x00;
				HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, ACCEL_CONFIG, 1, &data_check, 1, HAL_MAX_DELAY);
			}

	}

}


struct GyroData readGyro ()
{
		uint8_t data[6];
		int16_t raw_x = 0, raw_y = 0, raw_z = 0;
		struct GyroData gyro_res = {0};
		HAL_StatusTypeDef status = HAL_OK;

		status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, GYRO_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

		raw_x = (int16_t)(data[0] << 8 | data[1]);

		raw_y = (int16_t)(data[2] << 8 | data[3]);

		raw_z = (int16_t)(data[4] << 8 | data[5]);

		/* Converting raw values into degrees per second.
		 * Scaling by the sensitivity for the configured scale range.
		 */


		gyro_res.gyro_x = raw_x/kGryoCorrector;
		gyro_res.gyro_y= raw_y/kGryoCorrector;
		gyro_res.gyro_z= raw_z/kGryoCorrector;


		return gyro_res;

}


struct AccelData readAccel ()
{
		uint8_t data[6];
		int16_t raw_x = 0, raw_y = 0, raw_z = 0;

		struct AccelData accel_res = {0};
		HAL_StatusTypeDef status = HAL_OK;

		status = HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, ACCEL_XOUT_H, 1, data, 6, HAL_MAX_DELAY);

		raw_x = (int16_t)(data[0] << 8 | data[1]);

		raw_y = (int16_t)(data[2] << 8 | data[3]);

		raw_z = (int16_t)(data[4] << 8 | data[5]);

		/* Converting raw values into acceleration in terms of g.
		*  Scaling by the sensitivity for the configured scale range.
		*/

		accel_res.accel_x = raw_x/kAccelCorrector;
		accel_res.accel_y = raw_y/kAccelCorrector;
		accel_res.accel_z = raw_z/kAccelCorrector;


		return accel_res;


}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  imuInit();

//  checkReg();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  //read_gyro here
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	struct GyroData gyro_result = readGyro();
	struct AccelData accel_result = readAccel();
	HAL_Delay(10);



  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */


  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
