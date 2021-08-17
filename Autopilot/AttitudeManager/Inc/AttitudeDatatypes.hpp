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

#endif

