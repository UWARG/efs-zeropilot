#include "airspeed.hpp"
#include "adc.h"
#include <stddef.h>
#include <cmath>

#define VS 5
#define DENSITY_AIR 1.204
#define ADC_RESOLUTION 4096


MPXV7002DP::MPXV7002DP() {
    //constructor
    MX_ADC3_Init();
    //HAL_ADC_START_IT(&hadc3);
    //start the adc or whatever

    
    for(int x = 0; x < 100; x++) {
        initialPressure += getPressure();
    }
}

MPXV7002DP* MPXV7002DP::s_Instance = NULL;

MPXV7002DP* MPXV7002DP::GetInstance() {
    if(!s_Instance) {
        s_Instance = new MPXV7002DP;
    }
    return s_Instance;
}

void MPXV7002DP::GetResult(airspeedData_t *Data) {
        Data->airspeed = airspeed;
        Data->isDataNew = dataNew;
        dataNew = false;
        Data->sensorStatus = sensorStatus;
        Data->utcTime = timeOfLastResult;
}

void MPXV7002DP::Begin_Measuring() {
        airspeed = readFromMPXV7002DP();
        timeOfLastResult = getCurrentTime();
        dataNew = true;
}


//this entire function is super sus ngl
//gonna rewrite it lol
// double MPXV7002DP::readFromMPXV7002DP() {
//     uint16_t adcVal = HAL_ADC_GetValue(&hadc2) - offset;

//     if(adcVal < ADC_RESOLUTION/2) {
//         return -std::sqrt( (-10000*(adcVal/(ADC_RESOLUTION-1) - 0.5))/DENSITY_AIR );
//     } else if(adcVal > ADC_RESOLUTION/2) {
//         return std::sqrt( (10000*(adcVal/(ADC_RESOLUTION-1) - 0.5))/DENSITY_AIR );
//     } else {
//         return 0;
//     }
// }

float MPXV7002DP::getPressure() {
    uint16_t adcVal = HAL_ADC_GetValue(&hadc3) - offset;

    //formula from datasheet page 6
    return 5 * (float)adcVal/(ADC_RESOLUTION-1) - 2.5;
}

float MPXV7002DP::getAirspeed() {
    float pressure = getPressure();
    if(pressure < 0) {
        sensorStatus = 1;
        return 0;
    } else {
        return sqrt(2 * (pressure - offset)/DENSITY_AIR);
    }
}

uint32_t MPXV7002DP::getCurrentTime() {
    std::time_t result = std::time(0);
    return static_cast<int>(result);
}
