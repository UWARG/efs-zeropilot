#ifndef ATTITUDE_DATATYPES_HPP
#define ATTITUDE_DATATYPES_HPP

// Output of the PID module and input to the OutputMixing module
typedef struct
{
    float rollPercent;
    float pitchPercent;
    float rudderPercent;
    float throttlePercent;

} PID_Output_t;

#ifdef SPIKE
    #define L_TAIL_OUT_CHANNEL 0 // Spike has ruddervators
    #define R_TAIL_OUT_CHANNEL 1
#else
    #define ELEVATOR_OUT_CHANNEL 0
    #define RUDDER_OUT_CHANNEL 1
#endif

#define AILERON_OUT_CHANNEL 2
#define THROTTLE_OUT_CHANNEL 3

// New datatypes store the IMU and Airspeed data so SensorFusion and other modules
// do not need to include "IMU.hpp" and "airspeed.hpp"
struct IMU_Data_t
{
    float magx, magy, magz;
    float accx, accy, accz;
    float gyrx, gyry, gyrz; 

    bool isDataNew; 
    int sensorStatus; 
    float utcTime; 
};

struct Airspeed_Data_t
{
    double airspeed;        

    int sensorStatus;       
    bool isDataNew;         
    float utcTime;          
};

typedef struct {
    float pressure, altitude, temp;

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    float utcTime; //Last time GetResult was called
} Altimeter_Data_t;

typedef struct
{
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    float utcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
    float groundSpeed; // in m/s
    int altitude; // in m
    int heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
    int numSatellites;    // 1 Byte

    int sensorStatus; // 0 = no fix, 1 = gps fix, 2 = differential gps fix (DGPS) (other codes are possible)
    bool dataIsNew; // true if data has been refreshed since the previous time GetResult was called, false otherwise.

} Gps_Data_t;

#endif

