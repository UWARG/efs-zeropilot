/*
* Interchip packets
* Packets should be a multiple of 16 bits
*/
#ifndef INTERCHIP_H
#define INTERCHIP_H

typedef struct {
	uint16_t safetyLevel;
	int16_t PWM[12];
} Interchip_Packet;  

#endif