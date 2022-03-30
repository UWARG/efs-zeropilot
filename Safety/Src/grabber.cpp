
/* Includes ------------------------------------------------------------------*/
#include "grabber.hpp"
#include "stm32f0xx_hal_gpio.h"

/* Private define ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Public Methods  ---------------------------------------------------------*/

Grabber& Grabber::getInstance(const uint8_t GPIO_Channel){
  static Grabber singleton(GPIO_Channel); 
  return singleton; 
}

void Grabber::open() {
	if (!this->isOpen) {
		HAL_GPIO_WritePin(this->gpio, this->channel, GPIO_PIN_RESET);
	}
}

void Grabber::close() {
	if (this->isOpen) {
		HAL_GPIO_WritePin(this->gpio, this->channel, GPIO_PIN_SET);
	}
}

/* Private methods  ---------------------------------------------------------*/
Grabber::Grabber(uint8_t GPIO_Channel) {
	this->channel = GPIO_Channel;
	GPIO_InitTypeDef GPIO_InitStruct; //TODO: It might be better to init this in gpio.c. Also confirm these settings.
	GPIO_InitStruct.Pin = this->channel;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(this->gpio, &GPIO_InitStruct);
	HAL_GPIO_WritePin(this->gpio, this->channel, GPIO_PIN_RESET);

};
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
