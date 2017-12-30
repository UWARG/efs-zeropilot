#ifndef EEPROM_H
#define EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"

void EEPROM_Init(void);
void EEPROM_Write(uint16_t memAddress, uint8_t *data, uint16_t dataLen);
void EEPROM_Read(uint16_t memAddress, uint8_t *data, uint16_t dataLen);

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H */
