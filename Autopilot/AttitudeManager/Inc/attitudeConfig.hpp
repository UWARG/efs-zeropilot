#ifndef ATTITUDE_CONFIG_HPP
#define ATTITUDE_CONFIG_HPP

#include <cstdint>

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

constexpr static float ROLL_PID_KP {4.8};
constexpr static float ROLL_PID_KI {0};
constexpr static float ROLL_PID_KD {3};
constexpr static float ROLL_PID_INTEGRAL_MAX {0};
constexpr static float ROLL_PID_MIN {-100};
constexpr static float ROLL_PID_MAX {100};

constexpr static float PITCH_PID_KP {1.6};
constexpr static float PITCH_PID_KI {0}; 
constexpr static float PITCH_PID_KD {3.5};
constexpr static float PITCH_PID_INTEGRAL_MAX {0}; 
constexpr static float PITCH_PID_MIN {-100};
constexpr static float PITCH_PID_MAX {100};

constexpr static float YAW_PID_KP {1};
constexpr static float YAW_PID_KI {0}; 
constexpr static float YAW_PID_KD {0};
constexpr static float YAW_PID_INTEGRAL_MAX {0}; 
constexpr static float YAW_PID_MIN {-100};
constexpr static float YAW_PID_MAX {100};

constexpr static float AIRSPEED_PID_KP {1};
constexpr static float AIRSPEED_PID_KI {0}; 
constexpr static float AIRSPEED_PID_KD {0};
constexpr static float AIRSPEED_PID_INTEGRAL_MAX {0}; 
constexpr static float AIRSPEED_PID_MIN {0};
constexpr static float AIRSPEED_PID_MAX {100};

#endif
