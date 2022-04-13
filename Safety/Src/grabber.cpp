
/* Includes ------------------------------------------------------------------*/
#include "grabber.hpp"
#include "stm32f0xx_hal_gpio.h"

/* Private define ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Public Methods  ---------------------------------------------------------*/

Grabber& Grabber::getInstance(	uint8_t grabber_pwm_channel_1,
								uint8_t grabber_pwm_channel_2,
								uint8_t crane_pwm_channel_1,
								uint8_t crane_pwm_channel_2,
								PWMChannel *pwm) {
  static Grabber singleton(	grabber_pwm_channel_1,
							grabber_pwm_channel_2,
							crane_pwm_channel_1,
							crane_pwm_channel_2,
							pwm); 
  return singleton; 
}

// Grabber lowering is controlled based on this table:
// https://cdn-shop.adafruit.com/product-files/3190/drv8871.pdf
// DRV8871 - Section 7.3.1
// === H-Bridge Control ===
// | IN1 | IN2 | OUT1   | OUT2   | DESCRIPTION
// |-----|-----|--------|--------|-------------
// |  0  |  0  | High-Z | High-Z | Coast; H-bridge disabled to High-Z (sleep entered after 1 ms)
// |  0  |  1  | L      | H      | Reverse (Current OUT2 --> OUT1)
// |  1  |  0  | H      | L      | Forward (Current OUT1 --> OUT2)
// |  1  |  1  | L      | L      | Brake; low-side slow decay

void Grabber::crane_brake() {
	pwm.set(this->crane_channel_1, 100);
	pwm.set(this->crane_channel_2, 100);
}
void Grabber::crane_coast() {
	pwm.set(this->crane_channel_1, 0);
	pwm.set(this->crane_channel_2, 0);
}
// obviously this is not instant, and we probably need a callback once the device
// is finished lowering to set the motor into brake mode and update its state
// Unfortunately I dont know what that will look like... It could very well be time based
// need more details from Mech/Electrical
void Grabber::lower(uint8_t speed) {
	if (!this->isLowered) {
		pwm.set(this->crane_channel_1, speed);
		pwm.set(this->crane_channel_2, 0);
		this->isLowered = true;
	}
}
void Grabber::raise(uint8_t speed) {
	if (this->isLowered) {
		pwm.set(this->crane_channel_1, 0);
		pwm.set(this->crane_channel_2, speed);
		this->isLowered = false;
	}
}


//TODO: Still need data from electrical on the input to the motor driver, 
//		hopefully it is similar to the above
void Grabber::close(uint8_t speed) {
	if (!this->isOpen) {
		pwm.set(this->grabber_channel_1, speed);
		pwm.set(this->grabber_channel_2, 0);
		this->isOpen = true;
	}
}
void Grabber::open(uint8_t speed) {
	if (this->isOpen) {
		pwm.set(this->grabber_channel_1, 0);
		pwm.set(this->grabber_channel_2, speed);
		this->isOpen = false;
	}
}

/* Private methods  ---------------------------------------------------------*/
