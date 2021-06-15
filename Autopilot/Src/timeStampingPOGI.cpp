
#include "timeStampingPOGI.hpp"
#include "tim.h"
#include "stm32f7xx_hal_tim.h"


timeStampingPOGI::timeStampingPOGI(){

    //initializing and starting timer
    MX_TIM4_Init();
    HAL_TIM_Base_Init(&htim4);
    HAL_TIM_Base_Start(&htim4);

}


//just typical singleton stuff

//Allocates and initializes the static data member pointer (not the object itself)
timeStampingPOGI* timeStampingPOGI::s_Instance = NULL;

//creates the instance if it doesnt exist & returns the pointer to the instance
timeStampingPOGI* timeStampingPOGI::GetInstance(){

    if(s_Instance == NULL){
        s_Instance = new timeStampingPOGI();
    }

    return s_Instance;
}


void timeStampingPOGI::setGPSTime(GpsData_t* gpsData){

    //check if its new and set the recentUTCTime to it
    if (gpsData->timeIsNew){
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        recentUTCTime = gpsData->utcTime;
    }

}


//call this to receive the milliseconds since midnight (in UTC)
uint32_t timeStampingPOGI::getUTCTimeMS(){
    
    return (recentUTCTime*1000) + __HAL_TIM_GET_COUNTER(&htim4);
}








