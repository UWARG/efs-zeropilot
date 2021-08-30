/*
* Interchip packets
*/
#ifndef INTERCHIP_H
#define INTERCHIP_H

typedef struct {
	uint16_t safetyLevel;

    // to be converted to be int8_t
    // do we need signed integers? It seems like we might?
	int16_t PWM[12];
} Interchip_Packet;  
#endif