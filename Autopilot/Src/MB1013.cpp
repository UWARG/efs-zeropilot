#include "ultrasonic.h"
#include "adc.h"
#include <bits/stdint-uintn.h>
#include <stddef.h>
#include <cmath>

#define ADC_RESOLUTION 4096 
#define MILLIMETERS_TO_METERS = 1000

MB1013::MB1013() {
    MX_ADC3_Init();

    HAL_StatusTypeDef status = HAL_ADC_Start(&hadc3);
    if (status != HAL_OK) {
        // Houston, we have a problem
    }
    
}

MB1013* MB1013::staticInstance = NULL;
 
MB1013* MB1013::GetInstance() {
    if (!staticInstance) {
        s_instance = new MB1013;
    }
    return staticInstance;
}

void MB1013::beginMeasuring() {
    getDistance();
    
}

float MB1013::getDistance() {
    uint16_t adcVal = HAL_ADC_GetValue(&hadc3);

    // Each volt in the analog signal is 5 mm of the range, but that is from a range of 0 to 1023
    // Since the ADC is 12 bits (resolution of 4096) the adcVal needs to be scaled down by 4
    float distanceInMillimeters = (float) adcVal/(ADC_RESOLUTION*4) * 5; // Distance in millimeters
    float distanceInMeters = distanceInMillimeters*MILLIMETERS_TO_METERS;

    return distanceInMeters;
}

uint32_t MB1013:getCurrentTime() {
    std::time_t result = std::time(0);
    return static_cast<int>(result); // returns the time since the epoch
}

void MB1013::getData(ultrasonicData_t *Data) {
    Data -> distance = getDistance();
    Data -> utcTime = getCurrentTime();
    Data -> isDataNew = true;
    Data -> status = PASSING;
}