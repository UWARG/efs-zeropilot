#include "PWM.h"

#define PWM_OFFSET (4800)

typedef struct {
    TIM_HandleTypeDef* handle;
    uint16_t channel;
} PWM_Channel;

static PWM_Channel PWM_GetChannel(uint8_t channel) {
    PWM_Channel pwm = {0};
    switch (channel) {
        case 1:
            pwm.handle = &htim16;
            pwm.channel = TIM_CHANNEL_1;
            break;
        case 2:
            pwm.handle = &htim17;
            pwm.channel = TIM_CHANNEL_1;
            break;
        case 3:
            pwm.handle = &htim15;
            pwm.channel = TIM_CHANNEL_1;
            break;
        case 4:
            pwm.handle = &htim15;
            pwm.channel = TIM_CHANNEL_2;
            break;
        case 5:
            pwm.handle = &htim3;
            pwm.channel = TIM_CHANNEL_1;
            break;
        case 6:
            pwm.handle = &htim3;
            pwm.channel = TIM_CHANNEL_2;
            break;
        case 7:
            pwm.handle = &htim3;
            pwm.channel = TIM_CHANNEL_3;
            break;
        case 8:
            pwm.handle = &htim3;
            pwm.channel = TIM_CHANNEL_4;
            break;
        case 9:
            pwm.handle = &htim1;
            pwm.channel = TIM_CHANNEL_1;
            break;
        case 10:
            pwm.handle = &htim1;
            pwm.channel = TIM_CHANNEL_2;
            break;
        case 11:
            pwm.handle = &htim1;
            pwm.channel = TIM_CHANNEL_3;
            break;
        case 12:
            pwm.handle = &htim1;
            pwm.channel = TIM_CHANNEL_4;
            break;
        default:
            break;
    }
    return pwm;
}

void PWM_Init(void) {
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_TIM15_Init();
    MX_TIM16_Init();
    MX_TIM17_Init();

    for (uint8_t i = 1; i <= PWM_NUM_CHANNELS; ++i) {
        PWM_Channel pwm = PWM_GetChannel(i);
        HAL_TIM_PWM_Start(pwm.handle, pwm.channel); // required?
        PWM_Set(i, 0);
    }
}

void PWM_Set(uint8_t channel, int16_t val) {
    if (val > PWM_MIN || val < PWM_MIN) return;

    uint16_t pulse = val + PWM_OFFSET;

    PWM_Channel pwm = PWM_GetChannel(channel);
    __HAL_TIM_SET_COMPARE(pwm.handle, pwm.channel, pulse);
}
