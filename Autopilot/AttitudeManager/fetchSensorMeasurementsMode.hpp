/**
 * Fetch Sensor Measurements Mode: gets raw sensor data from the sensor drivers
 * Author: Dhruv Rawat
 * Last edited: January, 2021
 */ 

#include "IMU.hpp"
#include "airspeed.hpp"
#include "AttitudeDatatypes.hpp"

#ifndef FETCH_SENSOR_MEASUREMENTS_HPP
#define FETCH_SENSOR_MEASUREMENTS_HPP

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SensorError_t {
    int errorCode;
};

/**
 * Takes in sensor objects and the output data structures as parameters. 
 * This is the only module that interacts with the sensor drivers
 */ 
SensorError_t SensorMeasurements_GetResult(IMU *imusns, airspeed *airspeedsns, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata);

#endif

