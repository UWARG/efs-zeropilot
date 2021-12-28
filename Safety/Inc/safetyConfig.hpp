#ifndef SAFETY_CONFIG_HPP
#define SAFETY_CONFIG_HPP

// ppm channels so we can set and forget here.
// TODO: Add channels for Arm/Disarm (later also grabber controls)
constexpr uint8_t PITCH_PPM_CHANNEL {0};
constexpr uint8_t ROLL_PPM_CHANNEL {1};
constexpr uint8_t THROTTLE_PPM_CHANNEL {2};
constexpr uint8_t YAW_PPM_CHANNEL {3};

// pwm order: left tail, right tail, aileron, nose wheel, throttle, gimbal x, gimbal y, reverse thrust.
constexpr static uint8_t PWM_CHANNEL_MOTOR_1 {0};
constexpr static uint8_t PWM_CHANNEL_MOTOR_2 {1};
constexpr static uint8_t PWM_CHANNEL_MOTOR_3 {2};
constexpr static uint8_t PWM_CHANNEL_MOTOR_4 {3};


// PWM channels for the 4 motors of the quadcopter
constexpr static uint8_t FRONT_LEFT_MOTOR_CHANNEL {0};
constexpr static uint8_t FRONT_RIGHT_MOTOR_CHANNEL {1};
constexpr static uint8_t BACK_LEFT_MOTOR_CHANNEL {2};
constexpr static uint8_t BACK_RIGHT_MOTOR_CHANNEL {3};

#endif

