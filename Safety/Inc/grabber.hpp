
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GRABBER_HPP
#define GRABBER_HPP


/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx.h"
#include "PWM.hpp"
// #include <stdint.h>
// #include "../boardfiles/Inc/i2c.h"

/* Exported functions prototypes ---------------------------------------------*/

class Grabber {
public:
  static Grabber& getInstance(uint8_t grabber_pwm_channel_1,
                              uint8_t grabber_pwm_channel_2,
                              uint8_t crane_pwm_channel_1,
                              uint8_t crane_pwm_channel_2,
                              PWMChannel *pwm);
  Grabber(const Grabber*)=delete; 
  
  void brake();
  void open(uint8_t speed);
  void close(uint8_t speed);

  void crane_brake();
  void crane_coast();
  void lower(uint8_t speed);
  void raise(uint8_t speed);

  const uint8_t ppmCraneChannel = 6; // TODO: What channel?
  const uint8_t ppmClawChannel = 7; // TODO: What channel?

private:
  Grabber() {};
  Grabber(uint8_t grabber_pwm_channel_1,
          uint8_t grabber_pwm_channel_2,
          uint8_t crane_pwm_channel_1,
          uint8_t crane_pwm_channel_2,
          PWMChannel *pwm) : 
    grabber_channel_1(grabber_pwm_channel_1),
    grabber_channel_2(grabber_pwm_channel_2),
    crane_channel_1(grabber_pwm_channel_1),
    crane_channel_2(grabber_pwm_channel_2),
    pwm(*pwm) {}
  
  const uint8_t grabber_channel_1 = 0;
  const uint8_t grabber_channel_2 = 0;
  const uint8_t crane_channel_1 = 0;
  const uint8_t crane_channel_2 = 0;
  PWMChannel pwm;
};
#endif /* GRABBER_HPP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
