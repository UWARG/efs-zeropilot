#ifndef INC_INTERCHIP_HPP_
#define INC_INTERCHIP_HPP_

typedef struct {
	int16_t PWM[12];
	uint16_t safetyLevel;
} Interchip_Packet;    //Safety to Autopilot packet

void sendReceiveData();

int16_t getPWM(int8_t index);

uint16_t getSafetyLevel();
void setSafetyLevel(uint16_t level);

bool isDataNew();

void testSetup();

#endif /* INC_INTERCHIP_HPP_ */
