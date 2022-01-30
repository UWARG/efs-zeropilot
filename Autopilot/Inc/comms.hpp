#ifndef XBEE_HPP
#define XBEE_HPP

#define START                 0x7E
#define TRANSMIT_TYPE         0x10
#define RECIEVE_TYPE          0x90
#define RESPONSE_TYPE         0x8B

#define GROUND_ADDY           0x0013A20041B16D1C
#define DRONE_ADDY            0x0013A20041B16ECD
#define SHORT_ADDY            0xFFFE

#define BROADCAST_RADIUS      0x00
#define OPTIONS               0x00

// size 
#define TRANSMIT_PAYLOAD_SIZE 22
#define RECIEVE_PAYLOAD_SIZE  20

#define TRANSMIT_SIZE         40
#define RECIEVE_SIZE          40


class pogiData {
    public:
    uint8_t outputs[4]; 
    uint8_t grabberPos; 
    // 5
    float pitch; 
    float yaw; 
    float roll; 
    // 17
    uint8_t statusDisplay;
    // 18
    // use to send data as needed for debugging purposes only,
    // for changes going into the final version of this struct, make a new field in the struct
    uint8_t spareChannels[4];
    // 22
    void setArmState(bool arm); // 1 bit
    void setControlState(uint8_t state); // 2 bits
    void setPMState(uint8_t state); // 4 bits

    bool getArmState();
    uint8_t getControlState();
    uint8_t getPMState();
};

struct transmitFrame {
    uint8_t startDelim = START;
    uint16_t length = TRANSMIT_SIZE;
    uint8_t type = TRANSMIT_TYPE;
    uint8_t id;
    // 5 bytes

    uint64_t address64 = GROUND_ADDY;
    uint16_t address16 = SHORT_ADDY;
    //15

    uint8_t broadcastRadius = BROADCAST_RADIUS;
    uint8_t options = OPTIONS;
    // 17 bytes

    pogiData payload; //39 bytes

    uint8_t checksum; // 40 bytes

};

struct responseFrame {
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
    uint8_t rxBuffer[22];

    private:
    Comms();
    static Comms* instance;

};
void TestComms(void);
#endif
