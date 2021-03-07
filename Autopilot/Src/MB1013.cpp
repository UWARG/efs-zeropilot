#include "ultrasonic.h"
#include "adc.h"
#include <stddef.h>
#include <cmath>

#define SPEED_SOUND 343 // Speed of sound in the air is 343 m/s, used to calculate raw distance

MB1013::MB1013() {
    HAL_StatusTypeDef status = HAL_ADC_Start(&hadc3);

}