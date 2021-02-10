/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */

#include "AttitudeDatatypes.hpp"
#include "fetchSensorMeasurementsMode.hpp"

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

typedef struct {
    float IMUroll, IMUpitch, IMUyaw; //in rad (for now)
    float IMUrollrate, IMUpitchrate, IMUyawrate; //in rad/s (for now)

    float Airspeed; //in m/s (for now)
} SFAttitudeOutput_t;

typedef struct {
    float altitude;
} SFElevationOutput_t;

typedef struct {
    long double latitude; 
    long double longitude;
    float groundSpeed; // in m/s
    int16_t heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
} SFPositionOutput_t;

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
 * Get altitude data by fusing data from altimeter and GPS. 
 * (Altitude is synonymous with elevation)
 * @param[out]  Output  Reference to an output struct for elevation data.
 * @param[in]   altimeterdata
 * @param[in]   gpsdata
 */
SFError_t SF_GetElevation(SFElevationOutput_t *Output, AltimeterData_t *altimeterdata, GpsData_t *gpsdata);

/**
 * Get position data by fusing data from IMU and GPS.
 * @param[out]  Output  Reference to an output struct for position data.
 * @param[in]   imudata
 * @param[in]   gpsdata
 */
SFError_t SF_GetPosition(SFPositionOutput_t *Output, IMU_Data_t *imudata, GpsData_t *gpsdata);

#endif
