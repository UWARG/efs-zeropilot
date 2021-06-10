/*
* Interchip packets
*/
#ifndef INTERCHIP_H
#define INTERCHIP_H

typedef struct {
	uint16_t safetyLevel;
	int16_t PWM[12];
} Interchip_Packet;  

#endif