/*

Ultrasonic Sensor driver (HC-SR04) 
By: Dhruv Upadhyay
September 2021

*/


#ifndef ULTRASONIC_HPP
#define ULTRASONIC_HPP

#include "stdint.h"
#include "stdbool.h"
#include "stm32f765xx.h"
#include "stm32f7xx_hal_tim.h"

typedef struct ultrasonicData_t {
    float distance;
    bool isDataNew; // Records whether the data has been refreshed since the last time getResult was called
    GPIO_TypeDef * triggerPin;
    GPIO_TypeDef * echoPin;
    TIM_HandleTypeDef htim;
} ultrasonicData_t;
 
class ultrasonic {
    public:
        virtual void getDistance(ultrasonicData_t * data) = 0;
        static ultrasonicData_t data;
    
};

class HCSR04 : public ultrasonic {
    public:
        HCSR04(const HCSR04*) = delete;
        void getDistance(ultrasonicData_t * data);

    private:
        HCSR04();
        void trigger(GPIO_TypeDef * triggerPin, uint32_t pinName);
        void delayMicroseconds(uint32_t us);

};