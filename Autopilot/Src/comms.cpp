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

uint8_t calculateChecksum(uint8_t* data, uint8_t start, uint8_t end) {
    uint8_t checksum = 0;
    for (uint8_t i = start; i < end; i++) {
        checksum += data[i];
    }
    return checksum;
}

// this is a weird dumb no good way to do this, but I am tired and this works
// manually unwrapping the class because a cast seems to not work (internet says that classes have padding)
// will work on a v2 using only structs to get rid of those issues
void Comms::transmitMessage() {
    transmitFrame frame;
    frame.payload = this->tx;
    frame.id = 1;

    uint8_t rawData[40] = { 0 };
    uint8_t checksum = 0xFF;

    rawData[0] = frame.startDelim;

    uint8_t *castedVal = (uint8_t*)&frame.length;
    rawData[1] = castedVal[1];
    rawData[2] = castedVal[0];
    
    rawData[3] = frame.type;
    rawData[4] = frame.id;
    checksum -= frame.type;
    checksum -= frame.id;

    uint8_t index = 5;
    castedVal = (uint8_t*)&frame.address64;
    for(int i = 7; i > -1; i--) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[i];
        index++;
    }

    castedVal = (uint8_t*)&frame.address16;
    for (int i = 1; i > -1; i--) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[i];
        index++;
    }

    rawData[15] = frame.broadcastRadius;
    checksum -= frame.broadcastRadius;
    rawData[16] = frame.options;
    checksum -= frame.broadcastRadius;

    index = 17;
    for( int i = 0; i < 4; i++) {
        rawData[index] = frame.payload.outputs[i];
        checksum -= frame.payload.outputs[i];
        index++;
    }

    rawData[21] = frame.payload.grabberPos;
    index = 22;
    castedVal = (uint8_t*)&frame.payload.pitch;
    for( int i = 3; i > -1; i--) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[1];
        index++;
    }

    castedVal = (uint8_t*)&frame.payload.yaw;
    for( int i = 3; i > -1; i--) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[1];
        index++;
    }

    castedVal = (uint8_t*)&frame.payload.roll;
    for( int i = 3; i > -1; i--) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[1];
        index++;
    }

    rawData[34] = frame.payload.statusDisplay;
    checksum -= frame.payload.statusDisplay;

    index = 35;
    for( int i = 0; i < 4; i++) {
        rawData[index] = castedVal[i];
        checksum -= castedVal[1];
        index++;
    }

    rawData[39] = checksum;
    uint8_t size = sizeof(rawData);

    // send the data
    HAL_UART_Transmit(&huart1, rawData, size,100);
}
