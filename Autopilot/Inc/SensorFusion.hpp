/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

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
    float altitudeSpeed; //m/s
    long double latitude; //Decimal degrees
    float latitudeSpeed; //m/s
    long double longitude; //Decimal degrees
    float longitudeSpeed; //m/s
};

/**
 * Initiate sensor fusion.
 */ 
void SF_Init(void);

/**
 * Get fused sensor data.
 * @param[out]    output  Reference to an output struct for fused data.
 */ 
SFError_t SF_GetResult(SFOutput_t *output);

//RTOS stuff that the cpp files need
#ifdef TARGET_BUILD

extern "C"
{
#include "cmsis_os.h"
}

const char SF_MAIL_Q_SIZE = 1;

//Set up a mail queue for getting data from sensor fusion
extern osMailQDef(SFMailQ, SF_MAIL_Q_SIZE, SFOutput_t);
extern osMailQId SFMailQ;

#endif


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
