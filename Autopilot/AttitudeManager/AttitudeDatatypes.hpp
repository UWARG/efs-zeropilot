#ifndef ATTITUDE_DATATYPES_HPP
#define ATTITUDE_DATATYPES_HPP

// Output of the PID module and input to the OutputMixing module
typedef struct
{
    float rollPercent;
    float pitchPercent;
    float yawPercent;
    float throttlePercent;

} PID_Output_t;

#define L_TAIL_OUT_CHANNEL 0 // Spike has ruddervators
#define R_TAIL_OUT_CHANNEL 1
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

#endif

