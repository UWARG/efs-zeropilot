#ifndef SAFETY_CONFIG_HPP
#define SAFETY_CONFIG_HPP

// pwm order: left tail, right tail, aileron, nose wheel, throttle, gimbal x, gimbal y, reverse thrust.
constexpr static uint8_t PWM_CHANNEL_LEFT_TAIL {0};
constexpr static uint8_t PWM_CHANNEL_RIGHT_TAIL {1};
constexpr static uint8_t PWM_CHANNEL_AILERON {2};
constexpr static uint8_t PWM_CHANNEL_NOSE_WHEEL {3};
constexpr static uint8_t PWM_CHANNEL_THROTTLE {4};
constexpr static uint8_t PWM_CHANNEL_GIMBAL_YAW {5};
constexpr static uint8_t PWM_CHANNEL_GRABBER {6};

#endif

