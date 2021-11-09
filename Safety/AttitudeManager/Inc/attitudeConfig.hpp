#ifndef ATTITUDE_CONFIG_HPP
#define ATTITUDE_CONFIG_HPP

#include <cstdint>

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
