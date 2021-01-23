/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat
 */

#include "AttitudeDatatypes.hpp"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

struct SFOutput_t {
    float IMUroll, IMUpitch, IMUyaw; //in rad (for now)
    float IMUrollrate, IMUpitchrate, IMUyawrate; //in rad/s (for now)

    float Airspeed; //in m/s (for now)
};

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SFError_t{
    int errorCode;
};

/**
 * Method takes in the data from the imu and airspeed sensors along with a SFOutput_t reference.
 * This ensures SensorFusion does not have access to the sensor drivers
 */ 
SFError_t SF_GetResult(SFOutput_t *Output, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata);

#endif
