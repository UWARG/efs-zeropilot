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
    #define RUDDER_OUT_CHANNEL 3
#endif

#define AILERON_OUT_CHANNEL 1
#define THROTTLE_OUT_CHANNEL 2

#endif

