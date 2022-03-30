
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GRABBER_HPP
#define GRABBER_HPP


/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx.h"
// #include <stdint.h>
// #include "../boardfiles/Inc/i2c.h"

/* Exported functions prototypes ---------------------------------------------*/

class Grabber {
public:
  static Grabber& getInstance(uint8_t GPIO_Channel);
  Grabber(const Grabber*)=delete; 
  
  void open();
  void close();

  uint8_t ppmChannel = 5; // TODO: What channel?

private:
  Grabber() {};
  Grabber(uint8_t GPIO_Channel);

  GPIO_TypeDef *gpio = GPIOC; // TODO: Is this the right one?
  
  bool isOpen = false;
  uint8_t channel = 0;
};
#endif /* GRABBER_HPP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
