#ifndef SAFETY_CONTROLLER_H
#define SAFETY_CONTROLLER_H

/*
*  Call this function after PPM and PWM manager have been inited.
*/
void safety_controller_init();
void safety_controller_run();

#endif