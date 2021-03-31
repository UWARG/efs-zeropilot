/**
 * Fetch Sensor Measurements Mode: gets raw sensor data from the sensor drivers
 * Author: Avani Joshi & Laye Mehta
 * Last edited: March, 2021
 */ 
 
#include "altimeter.hpp"
#include "gps.hpp"
#include "pathDatatypes.hpp"

#ifndef GET_SENSOR_DATA_HPP
#define GET_SENSOR_DATA_HPP

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
SensorError_t Path_SensorMeasurements_GetResult(Altimeter *altimetersns, Gps *gpssns, Altimeter_Data_t *altimeterdata, Gps_Data_t *gpsdata);

#endif