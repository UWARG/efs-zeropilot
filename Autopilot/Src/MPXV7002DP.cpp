#include "airspeed.hpp"
#include "adc.h"
#include <stddef.h>

#include <cmath>

#define VS 5
#define DENSITY_AIR 1.2754
#define ADC_RESOLUTION 4096
#define KPA_TO_PA 1000
#define BASELINE 2048 // the real baseline should be 2047.5, but this is close enough

MPXV7002DP* MPXV7002DP::s_Instance = nullptr;

MPXV7002DP::MPXV7002DP() {
    //constructor
    HAL_StatusTypeDef status = HAL_ADC_Start(&hadc3);
    if(status != HAL_OK) {
        sensorStatus = 1;
    }
    CalcOffset();
}

MPXV7002DP* MPXV7002DP::GetInstance() {
    if(!s_Instance) {
        s_Instance = new MPXV7002DP;
    }
    return s_Instance;
}

uint16_t MPXV7002DP::GetAverage() {
	uint16_t avg = 0;
	for(int i = 0; i < 10; i++) {
		uint16_t val = HAL_ADC_GetValue(&hadc3);
		avg += val;
	}
	avg = avg/10;
	return avg;
}

void MPXV7002DP::GetResult(airspeedData_t &Data) {
    Data.airspeed = GetAirspeed();
    Data.isDataNew = true;
    Data.sensorStatus = sensorStatus;
}

void MPXV7002DP::Begin_Measuring() {
    
    airspeed = GetAirspeed();
    dataNew = true;
}

void MPXV7002DP::CalcOffset() {
	uint16_t avg = GetAverage();
	offset = avg - BASELINE;
}

float MPXV7002DP::GetDiffPressure() {
	float inputVoltage = GetVoltage();

    //formula from datasheet page 6
	float diffPressure = KPA_TO_PA * ( (5 * inputVoltage/VS) - 5.0/2.0);
    return diffPressure;
}

float MPXV7002DP::GetVoltage() {
	uint16_t adcVal = GetAverage() - this->offset;
	float voltage = ((float)(adcVal * VS))/(ADC_RESOLUTION - 1);
	return voltage;
}

float MPXV7002DP::GetAirspeed() {
	float dp = GetDiffPressure();

    if(dp < 0) {
        sensorStatus = 1;
        return 0.0;
    } else {
        sensorStatus = 0;
        double z = (2 * dp)/DENSITY_AIR;
        return sqrt(z);
    }
}
