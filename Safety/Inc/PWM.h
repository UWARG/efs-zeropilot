#ifndef PWM_H
#define PWM_H

#include "tim.h"

#define PWM_MAX (3200) // 500 us pulse
#define PWM_MIN (-3200) // 2500 us pulse

#define PWM_NUM_CHANNELS (12)

#define PPM_NUM_CHANNELS (8)

void PWM_Init(void);

void PWM_Set(uint8_t channel, int16_t val);

uint16_t PPM_Get(void);

#endif
