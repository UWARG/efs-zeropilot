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

void Comms::transmitMessage(pogiData tx) {
    uint8_t* msgByteArray = (uint8_t*)&tx;
    msgByteArray[sizeof(tx)] = calculateChecksum(msgByteArray,3,sizeof(tx)));
    HAL_UART_Transmit(&huart1, msgByteArray, sizeof(tx)+1,100); // fix the uart, not sure if I can use 1
}