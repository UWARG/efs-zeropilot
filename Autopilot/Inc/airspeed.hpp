/**
*   
*   Airspeed sensor functions
*
*   This header is an outline of basic functions of the airspeed
*   sensor. Generally, this sensor is a Pitot tube which uses a 
*   pressure differential to determine airspeed. 
*
*   Airspeed calculation should be done and returned in meters 
*   per second
*
*    Author: Annie LePage
**/


#ifndef AIRSPEED_HPP
#define AIRSPEED_HPP

#include<ctime>

/*
    Currently there is only one airspeed sensor used, if this is 
    changed to a different sensor, part numbers and a selection
    option should be added
*/

struct airspeedData_t 
{
    double airspeed;        // in m/s

    int sensorStatus;       // report any errors, possible malfunctions 
    bool isDataNew;         // is the data fresh?
    float utcTime;          // 4 Bytes. Time in seconds since 00:00 
};

class airspeed {
    public:
        /**
         *  Triggers interrupt for new airspeed measurement - stores 
         *  raw data in variables and returns right away
         * */
        virtual void Begin_Measuring() = 0; 

        /**GetResult should:
         *  1. Transfer raw data from variables to struct
         *  2. Update utcTime and status values in struct as well
         *  
         *
         *  Potentially:
         *  ensure that data acquired makes sense, has been
         *  gathered recently within reason (past 10s?)
         * */
        virtual void GetResult(airspeedData_t *Data) = 0; 
};

class dummyairspeed: public airspeed{
    public:

        /**
         *  Triggers interrupt for new airspeed measurement - stores 
         *  raw data in variables and returns right away
         * */
        void Begin_Measuring() {}; 

        /**GetResult should:
         *  1. Transfer raw data from variables to struct
         *  2. Update utcTime and status values in struct as well
         *  
         *
         *  Potentially:
         *  ensure that data acquired makes sense, has been
         *  gathered recently within reason (past 10s?)
         * */
        void GetResult(airspeedData_t &Data) {};

};

class MPXV7002DP : public airspeed {
    public:
        MPXV7002DP(const MPXV7002DP*) = delete;
        static MPXV7002DP* GetInstance();
        void GetResult(airspeedData_t *Data);
        void Begin_Measuring();
    private:
        MPXV7002DP();
        static MPXV7002DP* s_Instance; //single instance of airspeed
        //command to interface with MPXV7002
        double readFromMPXV7002DP();
        float getPressure();
        float getAirspeed();
        float initialPressure = 0;
        bool dataNew = false;
        double airspeed = 0; 
        int sensorStatus = 0; //Im not 100% sure how to check for errors in the device :(
        uint32_t timeOfLastResult = 0;
        uint32_t getCurrentTime();
};


#ifdef UNIT_TESTING
#include "airspeed_Mock.hpp"
#endif

#endif
