/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"
#include "fetchSensorMeasurementsMode.hpp"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

typedef struct {
    float IMUroll, IMUpitch, IMUyaw; //rad
    float IMUrollrate, IMUpitchrate, IMUyawrate; //rad/s
    float Airspeed; //m/s
} SFAttitudeOutput_t ;

typedef struct {
    float altitude; //m
    float altSpeed; //m/s
    long double latitude; //Decimal degrees
    float latSpeed; //m/s
    long double longitude; //Decimal degrees
    float longSpeed; //m/s
} SFPositionOutput_t;

const int NUM_KALMAN_VALUES = 6; 

typedef struct {
    float prevX[NUM_KALMAN_VALUES];
    float prevP[NUM_KALMAN_VALUES*NUM_KALMAN_VALUES];
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
SFError_t SF_GetAttitude(SFAttitudeOutput_t *Output, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata);

/**
 * Get position data by fusing data from IMU and GPS.
 * @param[out]    Output  Reference to an output struct for position data.
 * @param[in]     altimeterdata
 * @param[in]     gpsdata
 * @param[in]     imudata
 * @param[in,out] iterdata Iterative data from previous call which becomes input for next call.
 */
SFError_t SF_GetPosition(SFPositionOutput_t *Output, Altimeter_Data_t *altimeterdata, Gps_Data_t *gpsdata, IMU_Data_t *imudata, SFIterationData_t *iterdata);

#endif
