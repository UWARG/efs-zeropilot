/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
 */
#ifdef __cplusplus
extern "C"
{
#endif
    #include "CControlFunctions.h"
#ifdef __cplusplus
}
#endif

#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

//Frequency of SF calculations in Hz
const int SF_FREQ = 200;

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
typedef struct {
    int errorCode;
} SFError_t;

typedef struct {
    float roll, pitch, yaw; //rad
    float rollRate, pitchRate, yawRate; //rad/s
    float airspeed; //m/s
    float altitude; //m
    float rateOfClimb; //m/s
    long double latitude; //Decimal degrees
    float latitudeSpeed; //m/s
    long double longitude; //Decimal degrees
    float longitudeSpeed; //m/s
    double track; // degrees
    double heading; //degrees
} SFOutput_t;

//Following structs store the raw sensor data so other modules can have direct access to them without including sensor header files
typedef struct
{
    float magx, magy, magz;
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 

    bool isDataNew; 
    int sensorStatus; 
    float utcTime; 
} IMU_Data_t;

typedef struct 
{
    double airspeed;        

    int sensorStatus;       
    bool isDataNew;         
    float utcTime;          
} Airspeed_Data_t;

typedef struct
{
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    float utcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
    float groundSpeed; // in m/s
    short heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
    int altitude; // in m
    char numSatellites;    // 1 Byte
    char fixStatus; //0 = No GPS, 1 = GPS fix, 2 = DGSP Fix, 3 = Estimated/Dead Recoking Fix

    char sensorStatus; // 0 = no fix, 1 = gps fix, 2 = differential gps fix (DGPS) (other codes are possible)
    bool dataIsNew; // true if data has been refreshed since the previous time GetResult was called, false otherwise.
    bool timeIsValid;

    //Added these so autopilot knows which data is new
    bool ggaDataIsNew; //Position, altitude, time, and number of satellites
    bool vtgDataIsNew; //Groundspeed and Heading
}  Gps_Data_t;

typedef struct  {

    float pressure, altitude, temp;

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    int utcTime; //Last time GetResult was called
} Altimeter_Data_t;


/**
 * Initialize sensor fusion.
 */ 
void SF_Init(void);

/**
 * Generates fused sensor data. Should be called at a constant rate defined by SF_FREQ after SF_Init has been called once.
 */ 
SFError_t SF_GenerateNewResult();

/**
 * Get latest fused sensor data. Can be called any time data is needed after SF_init has been called once. Waits until the output struct is not being accessed by another task.
 * @param [out] output Output struct for fused data.
 * @return Error struct.
 */ 
SFError_t SF_GetResult(SFOutput_t *SFoutput);

/**
 * Get raw IMU data. Can be called any time raw data is needed.
 * @return IMU struct.
 */ 
IMU_Data_t SF_GetRawIMU();

/**
 * Get raw Airspeed data. Can be called any time raw data is needed.
 * @return Airspeed struct.
 */ 
Airspeed_Data_t SF_GetRawAirspeed();

/**
 * Get raw GPS data. Can be called any time raw data is needed.
 * @return GPS struct.
 */ 
Gps_Data_t SF_GetRawGPS();

/**
 * Get raw Altimeter data. Can be called any time raw data is needed.
 * @return Altimeter struct.
 */ 
Altimeter_Data_t SF_GetRawAltimeter();

#endif
