#include "../../Common/Inc/Interchip.h"

#ifndef INC_INTERCHIP_HPP_
#define INC_INTERCHIP_HPP_


// Starts interchip interrupt. Call this once, not in a loop, each interchip callback
// sets up another interrupt.
void interchipInit();

// Get the pwm signals that have been recieved. The length of the array returned will 
// always be 12.

// todo: change this to be int8_t as well
volatile int8_t *getPWM();


// todo: set safety level to uint8_t as well?!?!?

// Get the safety level that is currently being sent out.
uint16_t getSafetyLevel();

// Set the safety level that is currently being sent out.
void setSafetyLevel(uint16_t level);

// Return whether the data is new.
bool isDataNew();

// used to populate the interchip packet with fake data.
void testSetup();

#endif /* INC_INTERCHIP_HPP_ */
