/**
 * Xbee Sensor Functions
 * Author: Gordon Fountain
 */

#ifndef XBEE_HPP
#define XBEE_HPP

#include "TelemPathInterface.hpp" //Holds structs for sent and received data

//Ask Dhruv R (Elias) about UART stuff (Or ask our lord FREERTOS in the sky)

class XBEE{
    public:
        /**
         * Initializes XBEE
         * */
        virtual void Init() = 0; 

        /**
         * Triggers interrupt for new data retrieval by telemManager - stores raw data in variables and returns right away
         * */
        virtual void Receive_Data() = 0; 

        /**GetResult should:
         * 1. Reset newInput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        virtual void GetResult(PIGO *Data) = 0; //Switch to MavLink data (ask Jingting how to feed to Xbee)

        /*Interrupt to send data to ground       
        */
        virtual void Send_Data() = 0; //Only touched by FREERTOS

        /*SendResult should:
         * 1. Reset newOutput flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * 4. Wait for FREERTOS to call the Send_Data function
        */
        virtual void SendResult(POGI* Data) = 0;
};

#endif
