#ifndef EEPROM_H
#define EEPROM_H

#include "i2c.h"

void EEPROM_Init(void);
void EEPROM_Write(uint16_t memAddress, uint8_t *data, uint16_t dataLen);
void EEPROM_Read(uint16_t memAddress, uint8_t *data, uint16_t dataLen);


#endif /* EEPROM_H */
