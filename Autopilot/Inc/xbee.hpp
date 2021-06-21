/**
 * Xbee Sensor Functions
 * Author: Gordon Fountain
 */

#ifndef XBEE_HPP
#define XBEE_HPP

#include "TelemPathInterface.hpp" //Holds structs for sent and received data


typedef struct __xbee_in_message {
    uint8_t len;            // Length of payload
    uint8_t payload[255];
} xbee_in_t;


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
        xbee_in_t GetResult();

        /*Interrupt to send data to ground       
        */
        void Send_GS_Data(); //Only touched by FREERTOS

        /*SendResult should:
         * 1. Reset newOutput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * 4. Wait for FREERTOS to call the Send_Data function
        */
        void SendResult(uint8_t* data);
    
    private:
        XBEE();

        static XBEE* xbeeInstance;
};

#endif
