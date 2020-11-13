#include "airspeed.hpp"
#include "adc.h"
#include <stddef.h>
#include <cmath>

#define VS 5
#define DENSITY_AIR 1.2754
#define ADC_RESOLUTION 4096


MPXV7002DP::MPXV7002DP() {
    //constructor
    MX_ADC3_Init();
    HAL_ADC_START(&hadc3);
    //start the adc or whatever

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
        airspeed = getAirspeed();
        timeOfLastResult = getCurrentTime();
        dataNew = true;
}

float MPXV7002DP::getDiffPressure() {
    uint16_t adcVal = HAL_ADC_GetValue(&hadc3);

    //formula from datasheet page 6
    return 5 * (float)adcVal/(ADC_RESOLUTION-1) - 2.5;
}

float MPXV7002DP::getAirspeed() {
    float diffPressure = getDiffPressure();
    if(diffPressure < 0) {
        sensorStatus = 1;
        return 0;
    } else {
        sensorStatus - 0;
        return sqrt(2 * diffPressure/DENSITY_AIR);
    }
}

uint32_t MPXV7002DP::getCurrentTime() {
    std::time_t result = std::time(0);
    return static_cast<int>(result);
}
