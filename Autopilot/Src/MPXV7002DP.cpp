#include "airspeed.hpp"
#include "adc.h"
#include <stddef.h>
#include <cmath>

#define VS 5
#define DENSITY_AIR 1.204
#define ADC_RESOLUTION 4096


MPXV7002DP::MPXV7002DP() {
    //constructor
    //set up adc here? not sure
}

MPXV7002DP* MPXV7002DP::s_Instance = NULL;

MPXV7002DP* MPXV7002DP::GetInstance() {
    if(!s_Instance) {
        s_Instance = new MPXV7002DP;
    }
    return s_Instance;
}

void MPXV7002DP::GetResult(airspeedData_t &Data) {
        Data.airspeed = airspeed;
        Data.isDataNew = dataNew;
        dataNew = false;
        Data.sensorStatus = sensorStatus;
        Data.utcTime = timeOfLastResult;
}

void MPXV7002DP::Begin_Measuring() {
        airspeed = readFromMPXV7002DP();
        timeOfLastResult = getCurrentTime();
        dataNew = true;
        sensorStatus = 0;
}

void MPXV7002DP::Init() {

    //do I need to init the adc?

    uint16_t adcVal = HAL_ADC_GetValue(&hadc2);
    offset = adcVal - (ADC_RESOLUTION-1)/2;
}


double MPXV7002DP::readFromMPXV7002DP() {
    uint16_t adcVal = HAL_ADC_GetValue(&hadc2) - offset;

    if(adcVal < ADC_RESOLUTION/2) {
        return -std::sqrt( (-10000*(adcVal/(ADC_RESOLUTION-1) - 0.5))/DENSITY_AIR );
    } else if(adcVal > ADC_RESOLUTION/2) {
        return std::sqrt( (10000*(adcVal/(ADC_RESOLUTION-1) - 0.5))/DENSITY_AIR );
    } else {
        return 0;
    }

}

uint32_t MPXV7002DP::getCurrentTime() {
    std::time_t result = std::time(0);
    return static_cast<int>(result);
}