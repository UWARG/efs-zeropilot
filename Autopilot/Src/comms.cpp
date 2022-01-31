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

    rawData[0] = frame.startDelim;

    uint8_t *castedVal = (uint8_t*)&frame.length;
    rawData[1] = castedVal[1];
    rawData[2] = castedVal[0];
    rawData[3] = frame.type;
    rawData[4] = frame.id;

    castedVal = (uint8_t*)&frame.address64;
    rawData[5] = castedVal[7];
    rawData[6] = castedVal[6];
    rawData[7] = castedVal[5];
    rawData[8] = castedVal[4];
    rawData[9] = castedVal[3];
    rawData[10] = castedVal[2];
    rawData[11] = castedVal[1];
    rawData[12] = castedVal[0];

    castedVal = (uint8_t*)&frame.address16;
    rawData[13] = castedVal[1];
    rawData[14] = castedVal[0];

    rawData[15] = frame.broadcastRadius;
    rawData[16] = frame.options;

    rawData[17] = frame.payload.outputs[0];
    rawData[18] = frame.payload.outputs[1];
    rawData[19] = frame.payload.outputs[2];
    rawData[20] = frame.payload.outputs[3];

    rawData[21] = frame.payload.grabberPos;

    castedVal = (uint8_t*)&frame.payload.pitch;
    rawData[22] = castedVal[3];
    rawData[23] = castedVal[2];
    rawData[24] = castedVal[1];
    rawData[25] = castedVal[0];

    castedVal = (uint8_t*)&frame.payload.yaw;
    rawData[26] = castedVal[3];
    rawData[27] = castedVal[2];
    rawData[28] = castedVal[1];
    rawData[29] = castedVal[0];

    castedVal = (uint8_t*)&frame.payload.roll;
    rawData[30] = castedVal[3];
    rawData[31] = castedVal[2];
    rawData[32] = castedVal[1];
    rawData[33] = castedVal[0];

    rawData[34] = frame.payload.statusDisplay;

    rawData[35] = frame.payload.spareChannels[0];
    rawData[36] = frame.payload.spareChannels[1];
    rawData[37] = frame.payload.spareChannels[2];
    rawData[38] = frame.payload.spareChannels[3];

    rawData[39] = 0xFF - calculateChecksum(rawData, 3, 39);
    uint8_t size = sizeof(rawData);

    // send the data
    HAL_UART_Transmit(&huart1, rawData, size,100);
}
