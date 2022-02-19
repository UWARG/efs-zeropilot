#include "comms.hpp"
#include "usart.h"

uint8_t pogiData::getPMState() {
    return this->statusDisplay & 00001111;
}

uint8_t pogiData::getControlState() {
    return (this->statusDisplay & 00110000) >> 4;
}

bool pogiData::getArmState() {
     return (this->statusDisplay & 01000000) >> 6;
}

void pogiData::setArmState(bool arm) {
    this->statusDisplay = (this->statusDisplay & 00111111) | (arm << 6);
}

void pogiData::setControlState(uint8_t state) {
    this->statusDisplay = (this->statusDisplay & 01001111) | ((state & 00000011) << 4);
}

void pogiData::setPMState(uint8_t state) {
    this->statusDisplay = (this->statusDisplay & 01110000) | (state & 00001111);
}

Comms* Comms::instance = nullptr;

Comms::Comms() {
    //constructor
}

Comms* Comms::GetInstance() {
    if(!instance) {
        instance = new Comms;
    }
    return instance;
}

void Comms::transmitMessage(pogiData Data) {
    transmitFrame frame;
	frame.payload = Data;
	frame.id = 1;

	uint8_t rawData[42] = { 0 };
	uint8_t checksum = 0xFF;

	rawData[0] = frame.startDelim;

	uint8_t *castVal = (uint8_t*)&frame.length;
	rawData[1] = castVal[1];
	rawData[2] = castVal[0];

	rawData[3] = frame.type;
	rawData[4] = frame.id;
	checksum -= frame.type;
	checksum -= frame.id;

	uint8_t index = 5;
	castVal = (uint8_t*)&frame.address64;
	for(int i = 7; i > -1; i--) {
	    rawData[index] = castVal[i];
	    checksum -= castVal[i];
	    index++;
    }

	castVal = (uint8_t*)&frame.address16;
	for (int i = 1; i > -1; i--) {
	    rawData[index] = castVal[i];
	    checksum -= castVal[i];
	    index++;
	}

	rawData[15] = frame.broadcastRadius;
	checksum -= frame.broadcastRadius;
	rawData[16] = frame.options;
	checksum -= frame.options;

	   index = 17;
	castVal = (uint8_t*)&frame.payload;
	for(int i =0; i < 24; i++) {
	    rawData[index] = castVal[i];
	    checksum -= castVal[i];
	    index++;
	}

	rawData[41] = checksum;
	uint8_t size = sizeof(rawData);


	// send the data
	HAL_UART_Transmit(&huart1, rawData, size,100);
}
