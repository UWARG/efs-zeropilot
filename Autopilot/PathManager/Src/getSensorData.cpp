/**
 * Fetch Sensor Measurements Mode: gets raw sensor data from the sensor drivers
 * Author: Avani Joshi and Laye Mehta
 * Last edited: March, 2021
 */ 

#include "getSensorData.hpp"
#include <math.h>

SensorError_t SensorMeasurements_GetResult(Altimeter *altimetersns, Gps *gpssns, Altimeter_Data_t *altimeterdata, Gps_Data_t *gpsdata) {

    SensorError_t error;
    error.errorCode = 0;

    AltimeterData_t tempAltimeterdata;
    GpsData_t tempGpsData;
    
    //Retrieve raw IMU and Airspeed data
    altimetersns->GetResult(&tempAltimeterdata);
    gpssns->GetResult(&tempGpsData);

    // Copies values over to the attitude manager's sensor data structs 
    altimeterdata->pressure = tempAltimeterdata.pressure;
    altimeterdata->altitude = tempAltimeterdata.altitude;
    altimeterdata->temp = tempAltimeterdata.temp;

    altimeterdata->isDataNew = tempAltimeterdata.isDataNew;
    altimeterdata->status = tempAltimeterdata.status;
    altimeterdata->utcTime = tempAltimeterdata.utcTime;

    gpsdata->latitude = tempGpsData.latitude;
    gpsdata->longitude = tempGpsData.longitude;
    gpsdata->utcTime = tempGpsData.utcTime;
    gpsdata->groundSpeed = tempGpsData.groundSpeed;
    gpsdata->altitude = tempGpsData.altitude;
    gpsdata->heading = tempGpsData.heading;
    gpsdata->numSatellites = tempGpsData.numSatellites;
    gpsdata->sensorStatus = tempGpsData.sensorStatus;
    gpsdata->dataIsNew = tempGpsData.dataIsNew;


    //Abort if both sensors are busy or failed data collection
    if(altimeterdata->status != 0 || gpsdata->sensorStatus != 0)
    {  

        /************************************************************************************************
         * THIS WILL PUT THE STATE MACHINE INTO FATAL FAILURE MODE... WE NEED TO DECIDE IF THIS IS WHAT
         * WE WANT OR IF WE SHOULD RETHINK HOW WE WANT THIS MODULE TO RETURN A SENSOR ERROR! 
         ************************************************************************************************/

        error.errorCode = -1;
        return error;
    }

    //Check if data is old
    if(!altimeterdata->isDataNew || !gpsdata->dataIsNew){
        error.errorCode = 1;
    }

    return error;
}

