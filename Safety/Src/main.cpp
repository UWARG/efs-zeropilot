#include "main.hpp"
#include "Debug.hpp"
#include "safety_control.h"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "UART.hpp"
#include "stm32f0xx_hal.h"
#include "PWM.hpp"
#include "PPM.hpp"

char buffer[200]; //buffer for printing

void print_ppm_state(char* buffer, PPMChannel &ppm);

int main() {
	StatusCode status;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	//set clock sources
	initialize_system_clock();
	gpio_init();
	init_debug();

	info("\r\n\r\nStarting up...");
	sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
	info(buffer);

	PWMManager &manager = PWMManager::getInstance();
	status = manager.setup();
	info("PWMSetup", status);

	manager.channel(1).set(50);
	manager.channel(2).set(25);
	manager.channel(3).set(75);
	manager.channel(4).set(100);
	manager.channel(5).set(25);
	manager.channel(6).set(75);
	manager.channel(7).set(100);
	manager.channel(8).set(25);
	manager.channel(9).set(50);
	manager.channel(10).set(75);
	manager.channel(11).set(100);
	manager.channel(12).set(50);

	PPMChannel ppm;
	ppm.setNumChannels(8);
	ppm.setLimits(1, 1000,2000, 50);
	status = ppm.setup();
	ppm.setTimeout(200);
	info("PPM Setup", status);

	GPIOPin led1 = GPIOPin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led2 = GPIOPin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin led3 = GPIOPin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
	GPIOPin buzzer = GPIOPin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_OUTPUT, GPIO_STATE_LOW, GPIO_RES_NONE);
//	led1.setup();
//	led2.setup();
//	led3.setup();
//	buzzer.setup();
	led1.set_state(GPIO_STATE_LOW);
	led2.set_state(GPIO_STATE_LOW);

	//  Safety_Init();
	//  Safety_Run();

	UARTSettings port_settings;
	port_settings.rx_inverted = false;
	port_settings.timeout = 50;
	port_settings.stop_bits = 1;
	port_settings.cts_rts = false;
	port_settings.parity = UART_NO_PARITY; //double check this?
	port_settings.baudrate = 115200;

	UARTPort port = UARTPort(UART_PORT2, port_settings);

	status = port.setup();
	info("UART2 Setup", status);
	status = port.setupDMA(0, 24);
	info("UART2 DMA", status);

	uint8_t ubuffer[24];


	size_t bytes_read = 100;
	bool toggle = false;
	while (true) {

		status = port.read_bytes(ubuffer, 24, bytes_read);

		if (bytes_read > 0){
			debug_array("array" , ubuffer, bytes_read, false);
			debug_array("array" , ubuffer, bytes_read, true);
		}



		print_ppm_state(buffer, ppm);
		info(buffer);

		if (toggle) {
			//buzzer.set_state(GPIO_STATE_HIGH);
			led2.set_state(GPIO_STATE_LOW);
			toggle = false;
		} else {
			//buzzer.set_state(GPIO_STATE_LOW);
			led2.set_state(GPIO_STATE_HIGH);
			toggle = true;
		}

		sprintf(buffer, "System Time (ms): %lu", get_system_time());
		info(buffer);

		delay(1000);
	}
}

void print_ppm_state(char* buffer, PPMChannel &ppm){
	int len = sprintf(buffer,
			"CH1 (p, us): %d %lu\r\nCH2 (p, us): %d %lu\r\n"
			"CH3 (p, us): %d %lu\r\nCH4 (p, us): %d %lu\r\n"
			"CH5 (p, us): %d %lu\r\nCH6 (p, us): %d %lu\r\n"
			"CH7 (p, us): %d %lu\r\nCH8 (p, us): %d %lu\r\n",
			ppm.get(1), ppm.get_us(1),
			ppm.get(2), ppm.get_us(2),
			ppm.get(3), ppm.get_us(3),
			ppm.get(4), ppm.get_us(4),
			ppm.get(5), ppm.get_us(5),
			ppm.get(6), ppm.get_us(6),
			ppm.get(7), ppm.get_us(7),
			ppm.get(8), ppm.get_us(8));

	sprintf(&buffer[len], "PPM Disconnected? : %d\r\n", ppm.is_disconnected(get_system_time()));
}