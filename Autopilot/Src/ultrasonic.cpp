#include "ultrasonic.hpp"
#include "stm32f765xx.h"
#include "stm32f7xx_hal_tim.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f765xx.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"
#include <cstdint>

#define SPEED_OF_SOUND 0.034f  
#define TRIG_PIN   5 // temporary value, to be changed when pin configs completed
#define TIMER_MAX 0xffff // used to account for possible overflow values with the timer

// Defining pins for echo and trig pins re sensor
GPIO_TypeDef * triggerPin;
GPIO_TypeDef * echoPin;
TIM_HandleTypeDef * htim = htim11;



uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t difference = 0;
uint8_t isFirstCaptured = 0; // Is the first value captured
uint8_t distance = 0; // computed distance based on the length of the ECHO 

HCSR04 :: HCSR04() {
    HAL_TIM_IC_Start_IT(&htim11, TIM_CHANNEL_1); // Starts the timer (to be changed after pin configuration)
    // Set all local variables to default values:
    IC_Val1 = 0;
    IC_Val2 = 0;
    difference = 0;
    isFirstCaptured = 0; // Is the first value captured
    distance = 0; // computed distance based on the length of the ECHO 
}

HCSR04* HCSR04::ultrasonicInstance = nullptr;

HCSR04* HCSR04::GetInstance() {
    if (!ultrasonicInstance) {
        ultrasonicInstance = new HCSR04 {};
    }
    return ultrasonicInstance;
}

void HCSR04::trigger(GPIO_TypeDef * triggerPin, uint32_t pinName) {
    triggerPin -> BSRR |= (1 << TRIG_PIN); // set GPIO pin high
    this->delayMicroseconds(10); // Delay/wait for 10 us as defined in datasheet
    triggerPin -> BSRR |= (1 << (16 + TRIG_PIN)); // reset the pin, hence completing the sending of the pulse
}

void HCSR04::delayMicroseconds(uint32_t us) {
    int16_t currentTim = htim11.Instance -> CNT;
    while ((htim11.Instance -> CNT - currentTim) < us); // wait the required microseconds before ending
}

void HCSR04 :: getDistance(ultrasonicData_t * data) {
    trigger(triggerPin, TRIG_PIN); // Sends trigger pulse 
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_1) { // Channels arbitrarily set to 1 for now
        if (isFirstCaptured == 0) {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, HAL_TIM_ACTIVE_CHANNEL_1);
            isFirstCaptured = 1; 
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, HAL_TIM_ACTIVE_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (isFirstCaptured == 1) {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, HAL_TIM_ACTIVE_CHANNEL_1);
            isFirstCaptured = 0;
            __HAL_TIM_SET_COUNTER(htim, 0);

            if (IC_Val2 > IC_Val1) {
                difference = IC_Val2 - IC_Val1;
            }
            else if (IC_Val1 > IC_Val2) {
                difference = (0xffff - IC_Val1) + IC_Val2; // find difference between val1 and max, and add overflow val2 
            }
            distance = difference * SPEED_OF_SOUND/2;
            HCSR04 :: data.distance = distance; // update the struct with new data
            isFirstCaptured = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, HAL_TIM_ACTIVE_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1); // disable the interrupt after concluding the isr
        }
    }
}