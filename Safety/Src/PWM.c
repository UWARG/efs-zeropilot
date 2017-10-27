#include "PWM.h"
#include "debug.h"

#define PWM_OFFSET (4800)

typedef struct {
    TIM_HandleTypeDef* handle;
    uint16_t channel;
} PWM_Channel;

static __IO uint16_t ppm_last_capture = 0;
static __IO uint16_t capture_value[PPM_NUM_CHANNELS] = { 0 };
static __IO uint8_t ppm_index = 0;

static PWM_Channel PWM_GetChannel(uint8_t channel) {
    PWM_Channel pwm = { 0 };
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
    HAL_TIM_IC_Start_IT(&htim14, TIM_CHANNEL_1);

    for (uint8_t i = 1; i <= PWM_NUM_CHANNELS; ++i) {
        PWM_Channel pwm = PWM_GetChannel(i);
        HAL_TIM_PWM_Start(pwm.handle, pwm.channel);
        PWM_Set(i, 0);
    }
}

void PWM_Set(uint8_t channel, int16_t val) {
    if (val < PWM_MIN || val > PWM_MAX) return;

    uint16_t pulse = (uint16_t)(val + PWM_OFFSET);

    PWM_Channel pwm = PWM_GetChannel(channel);
    __HAL_TIM_SET_COMPARE(pwm.handle, pwm.channel, pulse);
}


uint16_t PPM_Get(void) {
    return ppm_last_capture;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM14) { // only timer configured for input capture
        uint16_t time_diff = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        if (ppm_index != 0) { // the first edge doesn't give us any data
            capture_value[ppm_index - 1] = time_diff;
        }
        ppm_index = (ppm_index + 1) % (PPM_NUM_CHANNELS + 1); // index should reset, but just in case
        __HAL_TIM_SET_COUNTER(htim, 0);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM14) { // only timer we care about
        ppm_index = 0;
    }
}
