#include "main.hpp"
#include "Debug.hpp"
#include "safety_control.h"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "UART.hpp"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_rcc.h"
#include "PWM.hpp"
#include "PPM.hpp"

extern UARTPort port;

int main() {
	StatusCode status;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//set clock sources
	initialize_system_clock();
	gpio_init();
	init_debug();

	info("\r\n\r\nStarting up...");
	char buffer[100];
	sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
	info(buffer);



	PWMManager &manager = PWMManager::getInstance();
	status = manager.setup();
	info("PWMSetup", status);

	PWMGroupSetting setting;

	setting.inverted = false;
	setting.max_length = 2000;
	setting.min_length = 1000;
	setting.period = 2000;
	manager.channel(1).set(50);
	manager.configure(PWM_GROUP_1, setting);
	manager.configure(PWM_GROUP_2, setting);
	manager.configure(PWM_GROUP_3_4, setting);
	manager.configure(PWM_GROUP_5_8, setting);
	manager.configure(PWM_GROUP_9_12, setting);

	manager.channel(2).set(25);
	manager.channel(3).set(75);
	manager.channel(4).set(100);
	manager.channel(5).set(50);
	manager.channel(6).set(50);
	manager.channel(7).set(50);
	manager.channel(8).set(50);
	manager.channel(9).set(50);
	manager.channel(10).set(50);
	manager.channel(11).set(50);
	manager.channel(12).set(50);





	PPMChannel ppm;

	ppm.setNumChannels(8);
	ppm.setLimits(1, 1000,2000);
	status = ppm.setup();

	info("PPM Setup", status);

	GPIOPin led1 = GPIOPin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led2 = GPIOPin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led3 = GPIOPin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);

	GPIOPin buzzer = GPIOPin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);

	led1.setup();
	led2.setup();
	led3.setup();
	buzzer.setup();

	led1.set_state(GPIO_STATE_LOW);
	led2.set_state(GPIO_STATE_LOW);

	//  Safety_Init();
	//  Safety_Run();

	uint32_t clock = HAL_RCC_GetHCLKFreq();
	clock = HAL_RCC_GetPCLK1Freq();

	bool test = false;
	while (1) {
		uint32_t  t =         HAL_RCC_GetSysClockFreq();
		uint32_t   t2=        HAL_RCC_GetHCLKFreq();
		uint32_t       t3 =   HAL_RCC_GetPCLK1Freq();

		sprintf(buffer, "PPMChannel1: %d", ppm.capture_value[3]);
		info(buffer);
//		if (test) {
//			//buzzer.set_state(GPIO_STATE_HIGH);
//			led2.set_state(GPIO_STATE_LOW);
//			test = false;
//		} else {
//			//buzzer.set_state(GPIO_STATE_LOW);
//			led2.set_state(GPIO_STATE_HIGH);
//			test = true;
//		}

		HAL_Delay(1000);
	}
}