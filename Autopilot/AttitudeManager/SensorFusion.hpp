/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong
 */
#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

//Include involved sensors
#include "IMU.hpp"
#include "airspeed.hpp"

struct SFOutput_t{
    float IMUroll, IMUpitch, IMUyaw; //in rad (for now)
    float IMUrollrate, IMUpitchrate, IMUyawrate; //in rad/s (for now)

    float Airspeed; //in m/s (for now)
};

struct SFError_t{
    int errorCode;
};

/**
 * Converts raw IMU output into roll, pitch and yaw angles
 * 
*/
void IMUconv(IMUData_t *imu);


SFError_t SF_GetResult(SFOutput_t *Output, SFError_t *Error);


#endif
