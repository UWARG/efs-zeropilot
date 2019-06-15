#include "Clock.hpp"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "wwdg.h"
#include "gpio.h"

#include "eeprom.h"
#include <stdio.h>
#include "Debug.hpp"

char buffer[200]; //buffer for printing

extern "C" void MX_FREERTOS_Init(void);

int main(void) {

	StatusCode status;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//set clock sources
	initialize_system_clock();
	gpio_init();
	init_debug(UART_PORT3);

	info("\r\n\r\nStarting up...");
	sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
	info(buffer);

	print_reset_state(buffer, get_reset_cause());
	info(buffer);

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_SPI5_Init();
//  MX_SPI4_Init();
//  MX_I2C1_Init();
//  MX_I2C4_Init();
//  MX_USART2_UART_Init();
//  MX_SPI1_Init();
//  MX_ADC3_Init();
//  MX_ADC2_Init();
//  MX_UART4_Init();
////  MX_USART3_UART_Init();
//  MX_USART1_UART_Init();
//  MX_SPI2_Init();
//  MX_TIM10_Init();
//  MX_TIM11_Init();
////  MX_WWDG_Init();
//  MX_CRC_Init();
//  MX_I2C2_Init();
	/* USER CODE BEGIN 2 */


//  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
//
//  EEPROM_Init();
//  /* USER CODE END 2 */
//
//  /* Call init function for freertos objects (in freertos.c) */
//  MX_FREERTOS_Init();

	/* Start scheduler */
//  osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	UARTSettings port_settings;
	port_settings.rx_inverted = false;
	port_settings.timeout = 50;
	port_settings.stop_bits = 1;
	port_settings.cts_rts = false;
	port_settings.parity = UART_NO_PARITY; //double check this?
	port_settings.baudrate = 115200;

	UARTPort port = UARTPort(UART_PORT4, port_settings);

	status = port.setup();
	info("UART2 Setup", status);
	status = port.setupDMA(0, 24);
	info("UART2 DMA", status);

	uint8_t ubuffer[100];

	size_t bytes_read = 100;

	uint64_t time;
	while (1) {
		time = get_system_time_us();
		sprintf(buffer, "Sys Time: %lu", (uint32_t) time);
//    debug(buffer);

		status = port.read_bytes(ubuffer, 24, bytes_read);

		if (bytes_read > 0) {
			debug_array("array", ubuffer, bytes_read, false);
			debug_array("array", ubuffer, bytes_read, true);
		}

//    delay(1000);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}