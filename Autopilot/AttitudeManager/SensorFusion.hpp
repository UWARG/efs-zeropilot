/**
 * Sensor Fusion - Converts raw sensor data into human readable formatted structs
 * Author: Lucy Gong
 */
#ifndef SENSORFUSION_HPP
#define SENSORFUSION_HPP

struct SFOutput_t{
    float IMUroll, IMUpitch, IMUyaw; //in rad (for now)
    float IMUrollrate, IMUpitchrate, IMUyawrate; //in rad/s (for now)

    float Airspeed; //in m/s (for now)
};

// -1 = FAILED
// 0 = SUCCESS
// 1 = Old Data
struct SFError_t{
    int errorCode;
};

SFError_t SF_GetResult(SFOutput_t *Output);


#endif
