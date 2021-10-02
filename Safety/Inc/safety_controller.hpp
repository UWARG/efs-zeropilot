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
bool Interchip_CRC_Checker(int data, uint32_t rx_crc);
uint16_t crc_calc_modbus(const uint8_t msgBuffer[], size_t len);
#endif
