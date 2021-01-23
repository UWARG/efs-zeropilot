#include "stm32f0xx_hal.h"
#include "main.h"
#include "PWM.hpp"
#define MAX_CHANNELS 10
#define NUM_AVAILABLE_CHANNELS 8

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

typedef struct PWMPinConfig {
	uint16_t num;
	GPIO_TypeDef* port;
	TIM_HandleTypeDef *timer;
	uint16_t timer_channel;
} PWMPinConfig;

static const PWMPinConfig PWM_CONFIG[MAX_CHANNELS] =
{
    {PWM1_Pin, PWM1_GPIO_Port, &htim1, TIM_CHANNEL_1},
    {PWM2_Pin, PWM2_GPIO_Port, &htim1, TIM_CHANNEL_2},
    {PWM3_Pin, PWM3_GPIO_Port, &htim1, TIM_CHANNEL_3},
    {PWM4_Pin, PWM4_GPIO_Port, &htim1, TIM_CHANNEL_4},
    {PWM6_Pin, PWM6_GPIO_Port, &htim3, TIM_CHANNEL_1},
    {PWM10_Pin, PWM10_GPIO_Port, &htim3, TIM_CHANNEL_2},
    {PWM11_Pin, PWM11_GPIO_Port, &htim3, TIM_CHANNEL_3},
    {PWM12_Pin, PWM12_GPIO_Port, &htim3, TIM_CHANNEL_4}

};

void PWMChannel::setup()
{
    for(int i = 0; i < NUM_AVAILABLE_CHANNELS; i++)
    {
        PWMPinConfig currentChannel = PWM_CONFIG[i];
        HAL_TIM_PWM_Start(currentChannel.timer,currentChannel.timer_channel);
    }
}

void PWMChannel::set(uint8_t channel, uint8_t percent)
{
    if(channel > MAX_CHANNELS || channel < 0)
    {
        channel = 0;
    }

    PWMPinConfig currentChannel = PWM_CONFIG[channel];
    uint32_t prescaler = (static_cast<TIM_HandleTypeDef *>(currentChannel.timer))->Init.Prescaler;
	uint32_t us = ((percent * (PWMChannel::max_signal - PWMChannel::min_signal)) / 100 + PWMChannel::min_signal);
    uint32_t periodTicks = (static_cast<TIM_HandleTypeDef *>(currentChannel.timer))->Init.Period;
	uint32_t ticks = static_cast<uint32_t>((static_cast<float>(us) / static_cast<float>(pwmPeriod)) * static_cast<float>(periodTicks));
    __HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *) currentChannel.timer, currentChannel.timer_channel, (uint32_t) ticks);

}
