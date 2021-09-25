#include "ultrasonic.hpp"
#include "stm32f765xx.h"
#include "stm32f7xx_hal_tim.h"
#include "stm32f7xx_hal_gpio.h"

#define SPEED_OF_SOUND 3.43f  

HCSR04 :: HCSR04() {
    HAL_TIM_IC_Start(&htim16, TIM_CHANNEL_1);
}