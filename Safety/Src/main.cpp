#include "main.hpp"
#include "Debug.hpp"
#include "safety_control.h"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "UART.hpp"
#include "stm32f0xx_hal.h"
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
	char buffer[200];
	sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
	info(buffer);

	PWMManager &manager = PWMManager::getInstance();
	status = manager.setup();
	info("PWMSetup", status);

	PWMGroupSetting setting;

	setting.inverted = false;
	setting.max_length = 2000;
	setting.min_length = 1000;
	setting.period = 2500;
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
	ppm.setLimits(1, 1000,2000, 50);
	status = ppm.setup();

	ppm.reset();
	ppm.setTimeout(200);
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

	UARTSettings uart2;
	uart2.baudrate = 115200;
	uart2.parity = UART_NO_PARITY;
	uart2.stop_bits = 1;
	uart2.cts_rts = false;
	uart2.tx_inverted = false;
	uart2.rx_inverted = false;
 	uart2.timeout = 5000;
	UARTPort serial = UARTPort(UART_PORT2, uart2);

	status = serial.setup();

	info("UART2 setup", status);

	char data[23];
	//status = serial.setupDMA(10,10);

	//info("UART2 DMA: ", status);
	size_t bytes_read = 100;


	//  Safety_Init();
	//  Safety_Run();


	bool test = false;
	while (1) {

		status = serial.read_bytes((uint8_t*)&data, 23, bytes_read);

		sprintf(buffer, "read bytes: %d", bytes_read);
		info(buffer, status);
		debug_array("uart2", (uint8_t*)&data, 23);
		debug_array("uart2", (uint8_t*)&data, 23, true);

//		sprintf(buffer,
//			"CH1 (p, us): %d %d\r\nCH2 (p, us): %d %d\r\n"
//   "CH3 (p, us): %d %d\r\nCH4 (p, us): %d %d\r\nCH5 (p, us): %d %d\r\nCH6 (p, us): %d %d\r\nCH7 (p, us): %d %d\r\nCH8 (p, us): %d %d\r\n",
//			ppm.get(1), ppm.get_us(1),
//		ppm.get(2), ppm.get_us(2),
//			ppm.get(3), ppm.get_us(3),
//			ppm.get(4), ppm.get_us(4),
//			ppm.get(5), ppm.get_us(5),
//			ppm.get(6), ppm.get_us(6),
//			ppm.get(7), ppm.get_us(7),
//			ppm.get(8), ppm.get_us(8));
//		info(buffer);
////		if (test) {
////			//buzzer.set_state(GPIO_STATE_HIGH);
////			led2.set_state(GPIO_STATE_LOW);
////			test = false;
////		} else {
////			//buzzer.set_state(GPIO_STATE_LOW);
////			led2.set_state(GPIO_STATE_HIGH);
////			test = true;
////		}
//		sprintf(buffer, "\r\nPPM Disconnected? : %d\r\n", ppm.is_disconnected(get_system_time()));
//		info(buffer);
//		sprintf(buffer, "System Time (ms): %u", get_system_time());
//		info(buffer);

		//delay(100);
	}
}