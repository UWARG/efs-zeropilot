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
         * */
        void Receive_GS_Data(); 

        /**GetResult should:
         * 1. Reset newInput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        void GetResult(Telemetry_PIGO_t &Data);

        /** 
         * Encode data in POGI struct and send data by calling Send_GS_Data();
         */
        _AirsideMavlinkDecodingEncoding SendResult(Telemetry_POGI_t &Data);
    
    private:
        XBEE();

        static XBEE* xbeeInstance;

        Telemetry_PIGO_t data;
        bool dataIsNew;
        std::vector<uint8_t> receivedData;

        /**
         * Called everytime we encode data. It sends data to the ground
         *
         * @param toSend -> string of bits to be sent to groundstation
         */
        void Send_GS_Data(uint8_t* toSend);

        



        
};

#endif
