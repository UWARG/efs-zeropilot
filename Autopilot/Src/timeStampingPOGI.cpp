#include "timeStampingPOGI.hpp"
#include "tim.h"
#include "stm32f7xx_hal_tim.h"

//Allocates and initializes the static data member pointer (not the object itself)
TimeStampingPOGI* TimeStampingPOGI::s_Instance = nullptr;

TimeStampingPOGI::TimeStampingPOGI(){

    //initializing and starting timer
    HAL_TIM_Base_Init(&htim14);
    HAL_TIM_Base_Start(&htim14);

}


//just typical singleton stuff
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

        __HAL_TIM_SET_COUNTER(&htim14, 0);

        //gps data sends time in the form HHMMSS.00 - will use integer division to extract the hours, minutes and seconds indiviudually
        int hours = gpsData->utcTime / 10000;
        int mins = (gpsData->utcTime - (hours * 10000)) / 100;
        int seconds = gpsData->utcTime - (hours * 10000) - (mins * 100);

        //now get UTC time in seconds (3600 seconds in an hour & 60 in a minute)
        recentUTCTime = (hours * 3600) + (mins * 60) + seconds;
    }

}


//call this to receive the milliseconds since midnight (in UTC)
uint32_t TimeStampingPOGI::getUTCTimeMS(){

    //the timer is set to tick every 0.25 milliseconds
    //thus, by dividing the number of ticks by 4, we get the milliseconds measured
    //we only need millisecond precision so we dont really care about the truncation
    return (recentUTCTime * 1000) + (__HAL_TIM_GET_COUNTER(&htim14) / 4);

}








