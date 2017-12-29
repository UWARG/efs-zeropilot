#ifndef PWM_H
#define PWM_H

#include "tim.h"

#define PPM_NUM_CHANNELS (8)

#define PPM_OFFSET 24000

void PWM_Init(void);

void PWM_Set(uint8_t channel, int16_t val);

void PWM_SetAll(int16_t *vals);

volatile int16_t* PPM_Get(void);

#endif
