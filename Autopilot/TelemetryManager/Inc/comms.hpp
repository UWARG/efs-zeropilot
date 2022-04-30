#ifndef XBEE_HPP
#define XBEE_HPP

#include "main.h"

#define START_BYTE            0x7E
#define TRANSMIT_TYPE         0x10
#define RECIEVE_TYPE          0x90
#define RESPONSE_TYPE         0x8B

#define GROUND_ADDR           0x0013A20041B16D1C
#define DRONE_ADDR            0x0013A20041B16ECD
#define SHORT_ADDR            0xFFFE

#define BROADCAST_RADIUS      0x00
#define OPTIONS               0x00

// size
#define TRANSMIT_PAYLOAD_SIZE 22
#define RECIEVE_PAYLOAD_SIZE  20

#define TRANSMIT_SIZE         36
#define RECIEVE_SIZE          40


struct __attribute__((packed)) pogiData {
    uint8_t outputs[4] = { 0 };
    uint8_t grabberPos = 0;
    float pitch = 0.0;
    float yaw = 0.0;
    float roll = 0.0;
    uint8_t statusDisplay = 0;

    float groundSpeed; // in m/s
    int altitude; // in m
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    
    // use to send data as needed for debugging purposes only,
    // for changes going into the final version of this struct, make a new field in the struct
    uint8_t spareChannels[4] = { 0 };
    
    void setArmState(bool arm); // 1 bit
    void setControlState(uint8_t state); // 2 bits
    void setPMState(uint8_t state); // 4 bits

    bool getArmState();
    uint8_t getControlState();
    uint8_t getPMState();
};  

struct __attribute__((packed)) transmitFrame {
    uint8_t startDelim = START_BYTE;
    uint16_t length = TRANSMIT_SIZE;
    uint8_t type = TRANSMIT_TYPE;
    uint8_t id;

    uint64_t address64 = GROUND_ADDR;
    uint16_t address16 = SHORT_ADDR;

    uint8_t broadcastRadius = BROADCAST_RADIUS;
    uint8_t options = OPTIONS;

    pogiData payload; 

    uint8_t checksum; 
};

struct __attribute__((packed)) responseFrame {
    uint8_t startDelim;
    uint16_t length;
    uint8_t type;
    uint8_t id;

    uint16_t address16;

    uint8_t txRetryCount;
    uint8_t deliveryStatus;
    uint8_t discoveryStatus;

    uint8_t checksum;
};

class Comms {
    public:
    Comms(const Comms*) = delete;
    static Comms* GetInstance();

    void transmitMessage();

    pogiData tx;
    //uint8_t rxBuffer[22];

    private:
    Comms();
    static Comms* instance;

};
void TestComms(void);
#endif