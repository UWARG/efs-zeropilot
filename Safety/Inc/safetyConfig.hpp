#ifndef SAFETY_CONFIG_HPP
#define SAFETY_CONFIG_HPP

// ppm channels so we can set and forget here.
constexpr uint8_t LONGITUDE_PPM_CHANNEL {0};
constexpr uint8_t LATITUDE_PPM_CHANNEL {1};
constexpr uint8_t VERTICAL_PPM_CHANNEL {2};
constexpr uint8_t YAW_PPM_CHANNEL {3};

// pwm order: left tail, right tail, aileron, nose wheel, throttle, gimbal x, gimbal y, reverse thrust.
constexpr static uint8_t PWM_CHANNEL_MOTOR_1 {0};
constexpr static uint8_t PWM_CHANNEL_MOTOR_2 {1};
constexpr static uint8_t PWM_CHANNEL_MOTOR_3 {2};
constexpr static uint8_t PWM_CHANNEL_MOTOR_4 {3};

#endif

