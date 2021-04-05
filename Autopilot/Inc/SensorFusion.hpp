/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"
#include "fetchSensorMeasurementsMode.hpp"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SFError_t{
    int errorCode;
};

struct SFOutput_t {
    float IMUroll, IMUpitch, IMUyaw; //rad
    float IMUrollrate, IMUpitchrate, IMUyawrate; //rad/s
    float Airspeed; //m/s
    float altitude; //m
    float altSpeed; //m/s
    long double latitude; //Decimal degrees
    float latSpeed; //m/s
    long double longitude; //Decimal degrees
    float longSpeed; //m/s
};

/**
 * Initiate sensor fusion.
 */ 
SFError_t SF_Init(void);

/**
 * Get fused sensor data.
 * @param[out]    output  Reference to an output struct for fused data.
 */ 
SFError_t SF_GetResult(SFOutput_t *output);




//TO BE DELETED - Temporary declarations to prevent build from breaking

typedef struct {
    float IMUroll, IMUpitch, IMUyaw; //rad
    float IMUrollrate, IMUpitchrate, IMUyawrate; //rad/s
    float Airspeed; //m/s
} SFAttitudeOutput_t ;

//The SF_Get functions ensure SensorFusion does not have access to the sensor drivers
/**
 * Get attitude data by fusing data from IMU and airspeed sensors.
 * @param[out]  Output  Reference to an output struct for attitude data.
 * @param[in]   imudata
 * @param[in]   airspeeddata
 */ 
SFError_t SF_GetAttitude(SFAttitudeOutput_t *Output, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata);

#endif
