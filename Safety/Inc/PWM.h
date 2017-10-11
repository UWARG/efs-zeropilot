#ifndef PWM_H
#define PWM_H

#include "tim.h"

#define MIN_PWM 1600
#define MAX_PWM 8000

void PWM_Init(void);

void PWM_Set(uint8_t channel, uint16_t pulse);

#endif
