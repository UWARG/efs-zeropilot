/**
 * Xbee Sensor Functions
 * Author: Gordon Fountain
 */

#ifndef XBEE_HPP
#define XBEE_HPP

#include "TelemPathInterface.hpp" //Holds structs for sent and received data
#include "Airside_Functions.hpp"
#include "Mavlink2_lib/common/mavlink.h"

#include <vector>

enum _AirsideMavlinkDecodingEncoding { MAVLINK_DECODING_ENCODING_OK = 0, MAVLINK_DECODING_ENCODING_FAILED };

class XBEE {
    public:
        static XBEE* getInstance();
        
        //make it a singleton by deleting constructor
        XBEE(const XBEE*) = delete;   

        /**
         * Triggers interrupt for new data retrieval by telemManager - stores raw data in variables and returns right away 
         */
        void Receive_GS_Data(); 

        /**
         * Called to retrieve the latest collected data 
         * 
         * @param Data -> data that will be taken back to the caller 
         */
        void GetResult(Telemetry_PIGO_t &Data);

        /** 
         * Encode data in POGI struct and send data by calling Send_GS_Data();
         * 
         * @param Data -> data we want to encode (send to ground)
         * 
         * @return error code
         */
        _AirsideMavlinkDecodingEncoding SendResult(Telemetry_POGI_t &Data);

        #ifdef UNIT_TESTING
            Telemetry_PIGO_t getData() { return data; }
            _AirsideMavlinkDecodingEncoding decodePacketBypass(uint8_t* toDecode) {
                return decodeMavlinkPacket(toDecode);
            }
        #endif 
    
    private:
        XBEE();

        static XBEE* xbeeInstance;
        void set_usart_dat_new(bool isNew);

        Telemetry_PIGO_t data;
        bool dataIsNew;
        std::vector<uint8_t> receivedData;
        uint8_t byte_collection_buffer[255];

        /**
         * Called everytime we encode data. It sends data to the ground
         *
         * @param toSend -> string of bits to be sent to groundstation
         */
        void Send_GS_Data(uint8_t* toSend);

        /**
         * Called to decode a packet
         * 
         * @param toDecode -> string of bits that will be decoded (is one entire mavlink packet)
         * 
         * @return error code
         */
         _AirsideMavlinkDecodingEncoding decodeMavlinkPacket(uint8_t* toDecode);
};

#endif
