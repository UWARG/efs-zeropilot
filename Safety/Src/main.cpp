#include "main.hpp"
#include "Debug.hpp"
#include "safety_control.h"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "UART.hpp"
#include "stm32f0xx_hal.h"
#include "PWM.hpp"

extern UARTPort port;

int main() {
	StatusCode status;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//set clock sources
	initialize_system_clock();
	gpio_init();
	init_debug();

	PWMManager &manager = PWMManager::getInstance();
	status = manager.setup();
//	PWMGroupSetting pset;
//	pset.period = 20000;
//	pset.min_length = 1000;
//	pset.max_length = 2000;
//	pset.inverted = false;
//	manager.configure(PWM_GROUP_1, pset);

	manager.channel(1).set(50);
	manager.channel(2).set(50);
	manager.channel(3).set(50);
	manager.channel(4).set(50);
	manager.channel(5).set(50);
	manager.channel(6).set(50);
	manager.channel(7).set(50);
	manager.channel(8).set(50);
	manager.channel(9).set(50);
	manager.channel(10).set(50);
	manager.channel(11).set(50);
	manager.channel(12).set(50);

	info("\r\n\r\nStarting up...");
	char buffer[100];
	sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
	info(buffer);

	GPIOPin led1 = GPIOPin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led2 = GPIOPin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led3 = GPIOPin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);

	GPIOPin buzzer = GPIOPin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);

	led1.setup();
	led2.setup();
	led3.setup();
	buzzer.setup();

	led1.set_state(GPIO_STATE_LOW);
	led2.set_state(GPIO_STATE_HIGH);

//  Safety_Init();
//  Safety_Run();

uint32_t clock = HAL_RCC_GetHCLKFreq();
clock = HAL_RCC_GetPCLK1Freq();

	bool test = false;
	while (1) {
		if (test) {
			//buzzer.set_state(GPIO_STATE_HIGH);
			led2.set_state(GPIO_STATE_LOW);
			test = false;
		} else {
			//buzzer.set_state(GPIO_STATE_LOW);
			led2.set_state(GPIO_STATE_HIGH);
			test = true;
		}

		HAL_Delay(1000);
	}
}