#include "PWM.h"

#define PWM_OFFSET (4800)

void PWM_Init(void) {
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_TIM15_Init();
    MX_TIM16_Init();
    MX_TIM17_Init();

    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); // required?

    // for (uint8_t i = 1, i <= PWM_NUM_CHANNELS; ++i) {
    //     PWM_Set(i, 0);
    // }
}

void PWM_Set(uint8_t channel, int16_t val) {
    if (val > MAX_PWM || val < MIN_PWM) return;

    uint16_t pulse = val + PWM_OFFSET;

    switch (channel) {
        case 1:
            __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, pulse);
            break;
        case 2:
            __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, pulse);
            break;
        case 3:
            __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, pulse);
            break;
        case 4:
            __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, pulse);
            break;
        case 5:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
            break;
        case 6:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
            break;
        case 7:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pulse);
            break;
        case 8:
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pulse);
            break;
        case 9:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
            break;
        case 10:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse);
            break;
        case 11:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pulse);
            break;
        case 12:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pulse);
            break;
        default:
            break;
    }
}
