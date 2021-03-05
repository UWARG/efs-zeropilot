/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"
#include "fetchSensorMeasurementsMode.hpp"
#include "arm_math.h"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP
struct SFOutput_t {
    float IMUroll, IMUpitch, IMUyaw; //in rad (for now)
    float IMUrollrate, IMUpitchrate, IMUyawrate; //in rad/s (for now)
    float Airspeed; //in m/s (for now)
};

typedef struct {
    float altitude;
    long double latitude; 
    long double longitude;
} SFPositionOutput_t;

typedef struct {
    arm_matrix_instance_f32 prevX;
    arm_matrix_instance_f32 prevP;
} SFIterationData_t;

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SFError_t{
    int errorCode;
};

//The SF_Get functions ensure SensorFusion does not have access to the sensor drivers
/**
 * Get attitude data by fusing data from IMU and airspeed sensors.
 * @param[out]  Output  Reference to an output struct for attitude data.
 * @param[in]   imudata
 * @param[in]   airspeeddata
 */ 
SFError_t SF_GetResult(SFOutput_t *Output, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata);

/**
 * Get position data by fusing data from IMU and GPS.
 * @param[out]    Output  Reference to an output struct for position data.
 * @param[in]     altimeterdata
 * @param[in]     gpsdata
 * @param[in]     imudata
 * @param[in,out] iterdata Iterative data from previous call which becomes input for next call.
 * @param[in]     dt  Time since last call.
 */
SFError_t SF_GetPosition(SFPositionOutput_t *Output, AltimeterData_t *altimeterdata, GpsData_t *gpsdata, IMU_Data_t *imudata, SFIterationData_t *iterdata, float dt);

#endif
