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

/** new_safety_run
 * Grabs newest PPM values or target from Path Manager and updates target location for Controls.
 * @param [PWMChannel &pwm] PWMChannel. To be replaced with Path Manager instructions.
 * @param [PPMChannel &ppm] PPMChannel from RC Control.
 */
void new_safety_run(PWMChannel &pwm, PPMChannel &ppm);
void safety_run(PWMChannel &pwm, PPMChannel &ppm);
bool isSafetyManual();

#endif
