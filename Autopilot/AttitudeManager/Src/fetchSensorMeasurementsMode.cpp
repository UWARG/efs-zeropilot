/**
 * Fetch Sensor Measurements Mode: gets raw sensor data from the sensor drivers
 * Author: Dhruv Rawat
 * Last edited: January, 2021
 */ 

#include "fetchSensorMeasurementsMode.hpp"
#include <math.h>

SensorError_t SensorMeasurements_GetResult(IMU *imusns, airspeed *airspeedsns, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata) {

    SensorError_t error;
    error.errorCode = 0;

    // Structures from sensor driver files
    /* 
        Why am I making these? 

        When cleaning up the Attitude Manager, one of the goals was to ensure that only a select few modules have access to the senor driver files (IMU.hpp, etc.)
        To accomplish this, new structs to store the sensor data were created and declared in AttitudeDatatypes.hpp. These structs ensure that moduels (ex. Sensor Fusion)
        that need access to sensor data do not need to inclued the sensor driver header files. Unfortunately, the sensor drivers only accept the structs declared in the sensor
        driver header files, so we need to declare these temporary data structures to get the sensor data. 
    */
    IMUData_t tempIMUdata;
    airspeedData_t tempAirspeedData;

    //Retrieve raw IMU and Airspeed data
    imusns->GetResult(tempIMUdata);
    airspeedsns->GetResult(tempAirspeedData);

    // Copies values over to the attitude manager's sensor data structs 
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

    return error;
}

