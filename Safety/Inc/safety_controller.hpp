#ifndef SAFETY_CONTROLLER_H
#define SAFETY_CONTROLLER_H
#include "PPM.hpp"

/*
*  Call this function after PPM and PWM manager have been inited.
*/
void safety_controller_init();

/*
*  This function needs to get called in the main while loop for constant polling
*/
void safety_run(PWMChannel &pwm, PPMChannel &ppm);
bool isSafetyManual();

#endif
