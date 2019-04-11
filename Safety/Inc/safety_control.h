
#ifndef SAFETY_CONTROL_H
#define SAFETY_CONTROL_H

#include <stdlib.h>
#include "interchip_S.h"
#include "PWM.h"
#include "spi.h"

void Safety_Init(void);
void Safety_Run(void);

#endif  // SAFETY_CONTROL_H
