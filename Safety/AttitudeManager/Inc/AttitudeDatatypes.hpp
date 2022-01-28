#ifndef ATTITUDE_DATATYPES_HPP
#define ATTITUDE_DATATYPES_HPP

#include <cstdint>

// Output of the PID module and input to the OutputMixing module
typedef struct
{
    float frontLeftMotorPercent;
    float frontRightMotorPercent;
    float backLeftMotorPercent;
    float backRightMotorPercent;

} PID_Output_t;

constexpr static uint8_t FRONT_LEFT_MOTOR_CHANNEL {0};
constexpr static uint8_t FRONT_RIGHT_MOTOR_CHANNEL {1};
constexpr static uint8_t BACK_LEFT_MOTOR_CHANNEL {2};
constexpr static uint8_t BACK_RIGHT_MOTOR_CHANNEL {3};

constexpr static uint8_t NUM_PWM_CHANNELS {4}; // keeping them at 4 motors only for now, will def be changed as more elements added

/*
Commented out for now to allow for just quadcopter channel mapping
*/

// #ifdef SPIKE
//     constexpr static uint8_t L_TAIL_OUT_CHANNEL {0}; // Spike has ruddervators
//     constexpr static uint8_t R_TAIL_OUT_CHANNEL {1};
// #else
//     constexpr static uint8_t ELEVATOR_OUT_CHANNEL {0};
//     constexpr static uint8_t RUDDER_OUT_CHANNEL {1};
// #endif

// constexpr static uint8_t AILERON_OUT_CHANNEL {2};
// constexpr static uint8_t THROTTLE_OUT_CHANNEL {3};

// constexpr static uint8_t NUM_PWM_CHANNELS {8};

<<<<<<< HEAD
#endif // ATTITUDE_DATATYPES_HPP
=======
// #endif
>>>>>>> 2ac372122bb825aa8f51705d39eab6195a5f3817
