#ifndef TIME_STAMPING_POGI_HPP
#define TIME_STAMPING_POGI_HPP

#include <cstdint>
#include <cstdlib>
#include "gps.hpp"

class TimeStampingPOGI {

    public:

        /*
        *Called to either create or get the existing instance of the class
        *Constructor is private so it only gets called through here (singleton)
        */
        static TimeStampingPOGI* GetInstance();

        /*
        * Called by sensor fusion to give the most recent gpsData struct
        * This is used to update the recent UTC time in seconds
        */
        void setGPSTime(GpsData_t* gpsData); 

        /*
        * Call at any give time to receive the UTC time
        * Given in milliseconds since midnight 
        */
        uint32_t getUTCTimeMS();

    private:

        /*
        * Just the basic constructor - private so its a singleton
        */
        TimeStampingPOGI(); 

        /*
        * A pointer to the existing class instance - nullptr if no instance exists
        */
        static TimeStampingPOGI* s_Instance;

        /*
        * The most recent UTC time in seconds since midnight
        */
        uint32_t recentUTCTime;


};

#endif


