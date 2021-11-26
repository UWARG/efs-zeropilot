#ifndef ATTITUDE_DATATYPES_HPP
#define ATTITUDE_DATATYPES_HPP

#include <cstdint>

// Output of the PID module and input to the OutputMixing module
typedef struct
{
    float motor1Percent;
    float motor2Percent;
    float motor3Percent;
    float motor4Percent;

} PID_Output_t;

#ifdef SPIKE
    constexpr static uint8_t L_TAIL_OUT_CHANNEL {0}; // Spike has ruddervators
    constexpr static uint8_t R_TAIL_OUT_CHANNEL {1};
#else
    constexpr static uint8_t ELEVATOR_OUT_CHANNEL {0};
    constexpr static uint8_t RUDDER_OUT_CHANNEL {1};
#endif

constexpr static uint8_t AILERON_OUT_CHANNEL {2};
constexpr static uint8_t THROTTLE_OUT_CHANNEL {3};

constexpr static uint8_t NUM_PWM_CHANNELS {8};

#endif

