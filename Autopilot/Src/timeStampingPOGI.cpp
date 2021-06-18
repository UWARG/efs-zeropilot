
#include "TimeStampingPOGI.hpp"
#include "tim.h"
#include "stm32f7xx_hal_tim.h"


TimeStampingPOGI::TimeStampingPOGI(){

    //initializing and starting timer
    MX_TIM12_Init();
    HAL_TIM_Base_Init(&htim12);
    HAL_TIM_Base_Start(&htim12);

}


//just typical singleton stuff

//Allocates and initializes the static data member pointer (not the object itself)
TimeStampingPOGI* TimeStampingPOGI::s_Instance = nullptr;

//creates the instance if it doesnt exist & returns the pointer to the instance
TimeStampingPOGI* TimeStampingPOGI::GetInstance(){

    if(s_Instance == nullptr){
        s_Instance = new TimeStampingPOGI();
    }

    return s_Instance;
}


void TimeStampingPOGI::setGPSTime(GpsData_t* gpsData){

    //check if its new and set the recentUTCTime to it
    if (gpsData->timeIsNew){
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        recentUTCTime = gpsData->utcTime;
    }

}


//call this to receive the milliseconds since midnight (in UTC)
uint32_t TimeStampingPOGI::getUTCTimeMS(){

    return (recentUTCTime * 1000) + __HAL_TIM_GET_COUNTER(&htim4);

}








