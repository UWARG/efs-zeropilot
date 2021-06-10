#include "../../Common/Inc/Interchip.h"
#ifndef INC_INTERCHIP_HPP_
#define INC_INTERCHIP_HPP_


void sendReceiveData();

int16_t getPWM(int8_t index);

uint16_t getSafetyLevel();
void setSafetyLevel(uint16_t level);

bool isDataNew();

void testSetup();

#endif /* INC_INTERCHIP_HPP_ */
