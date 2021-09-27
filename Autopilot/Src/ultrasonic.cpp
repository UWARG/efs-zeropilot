#include "ultrasonic.hpp"
#include "stm32f765xx.h"
#include "stm32f7xx_hal_tim.h"
#include "stm32f7xx_hal_gpio.h"
#include "tim.h"
#include <cstdint>

#define SPEED_OF_SOUND 3.43f  
#define TRIG_PIN   5 // temporary value, to be changed when pin configs completed

HCSR04 :: HCSR04() {
    HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1); // Starts the timer (to be changed after pin configuration)
}

void trigger(GPIO_TypeDef * triggerPin, uint32_t pinName) {
    triggerPin -> BSRR |= (1 << TRIG_PIN); // set GPIO pin high
    delayMicroseconds(10); // Delay/wait for 10 us as defined in datasheet
    triggerPin -> BSRR |= (1 << (16 + TRIG_PIN)); // reset the pin, hence completing the sending of the pulse
}

void delayMicroseconds(uint32_t us) {
    int16_t currentTim = htim16.Instance -> CNT;
    while ((htim16.Instance -> CNT - currentTim) < delay); // wait the required microseconds before ending
}

HCSR04 :: getDistance(ultrasonicData_t * data) {
    trigger(data -> triggerPin, TRIG_PIN); // Sends trigger pulse 
    

}