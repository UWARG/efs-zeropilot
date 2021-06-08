/*
 * Interchip.hpp
 *
 *  Created on: Jun 4, 2021
 *      Author: aadi
 */

#ifndef INC_INTERCHIP_HPP_
#define INC_INTERCHIP_HPP_

typedef struct {
	int16_t PWM[12];
	uint16_t safetyLevel;
} Interchip_Packet;    //Safety to Autopilot packet

void init();

bool sendRecieveData();

bool setPWM(int8_t index,int16_t value);
int16_t getPWM(int8_t index);

uint16_t getSafetyLevel();

bool isDataNew();

#endif /* INC_INTERCHIP_HPP_ */
