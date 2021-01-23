/**
 * Fetch Sensor Measurements Mode: gets raw sensor data from the sensor drivers
 * Author: Dhruv Rawat
 * Last edited: January, 2021
 */ 

#include "fetchSensorMeasurementsMode.hpp"
#include <math.h>

#include <iostream>

SensorError_t SensorMeasurements_GetResult(IMU *imusns, airspeed *airspeedsns, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata) {

    SensorError_t error;
    error.errorCode = 0;

    // Structures from sensor driver files
    IMUData_t tempIMUdata;
    airspeedData_t tempAirspeedData;

    //Retrieve raw IMU and Airspeed data
    imusns->GetResult(tempIMUdata);
    airspeedsns->GetResult(tempAirspeedData);

    // Copies values over
    imudata->gyrx = tempIMUdata.gyrx;
    imudata->gyry = tempIMUdata.gyry;
    imudata->gyrz = tempIMUdata.gyrz;

    imudata->accx = tempIMUdata.accx;
    imudata->accy = tempIMUdata.accy;
    imudata->accz = tempIMUdata.accz;

    imudata->magx = tempIMUdata.magx;
    imudata->magy = tempIMUdata.magy;
    imudata->magz = tempIMUdata.magz;

    imudata->isDataNew = tempIMUdata.isDataNew;
    imudata->sensorStatus = tempIMUdata.sensorStatus;
    imudata->utcTime = tempIMUdata.utcTime;

    airspeeddata->airspeed = tempAirspeedData.airspeed;
    airspeeddata->sensorStatus = tempAirspeedData.sensorStatus;
    airspeeddata->isDataNew = tempAirspeedData.isDataNew;
    airspeeddata->utcTime = tempAirspeedData.utcTime;

    //Abort if both sensors are busy or failed data collection
    if(imudata->sensorStatus != 0 || airspeeddata->sensorStatus != 0)
    {  

        /************************************************************************************************
         * THIS WILL PUT THE STATE MACHINE INTO FATAL FAILURE MODE... WE NEED TO DECIDE IF THIS IS WHAT
         * WE WANT OR IF WE SHOULD RETHINK HOW WE WANT THIS MODULE TO RETURN A SENSOR ERROR! 
         ************************************************************************************************/

        error.errorCode = -1;
        return error;
    }

    //Check if data is old
    if(!imudata->isDataNew || !airspeeddata->isDataNew){
        error.errorCode = 1;
    }

    // Checks if magnetometer values are not a number (NAN) and converts them to zero if they are (ensures Madgwick does not break)
    // NOTE TO FUTURE DEVELOPERS: At the time of making, our IMU did not have a magnetometer (so for now we set the values to NAN). 
    // If your IMU does have one, you can remove this
    if (isnan(imudata->magx)) {
        imudata->magx = 0.0f;
    }
    if (isnan(imudata->magy)) {
        imudata->magy = 0.0f;
    }
    if (isnan(imudata->magz)) {
        imudata->magz = 0.0f;
    }

    return error;
}

