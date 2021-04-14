/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"

extern "C"
{
    #include "CControlFunctions.h"
}

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

//Frequency of SF calculations in Hz
constexpr int SF_FREQ = 512;

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SFError_t{
    int errorCode;
};

struct SFOutput_t {
    float roll, pitch, yaw; //rad
    float rollRate, pitchRate, yawRate; //rad/s
    float airspeed; //m/s
    float altitude; //m
    float rateOfClimb; //m/s
    long double latitude; //Decimal degrees
    float latitudeSpeed; //m/s
    long double longitude; //Decimal degrees
    float longitudeSpeed; //m/s
};

/**
 * Initialize sensor fusion.
 */ 
void SF_Init(void);

/**
 * Generates fused sensor data. Should be called at a constant rate defined by SF_FREQ after SF_Init has been called once.
 */ 
SFError_t SF_GenerateNewResult();

/**
 * Get latest fused sensor data. Can be called any time data is needed after SF_init has been called once. Waits until the output struct is not being accessed by another task.
 * @return Output struct for fused data.
 */ 
SFError_t SF_GetResult(SFOutput_t *output);




#endif
