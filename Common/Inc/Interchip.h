/*
* Interchip packets
*/
#ifndef INTERCHIP_H
#define INTERCHIP_H

typedef struct {
    // todo: set safety level to uint8_t
    uint8_t safetyLevel;

    // to be converted to be int8_t
    // do we need signed integers? It seems like we might?
    int16_t PWM[12];
    uint16_t crc;
} Interchip_Packet;
#endif