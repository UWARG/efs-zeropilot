#ifndef TIME_STAMPING_POGI_HPP
#define TIME_STAMPING_POGI_HPP

#include <cstdint>
#include <cstdlib>
#include "gps.hpp"

class timeStampingPOGI {

    public:

        static timeStampingPOGI* GetInstance();
        void setGPSTime(GpsData_t* gpsData); //need to change data type depending on what sensorFusion sends me
        uint32_t getUTCTimeMS();

    private:

        timeStampingPOGI(); //constructor cant be called - singleton
        static timeStampingPOGI* s_Instance;
        uint32_t recentUTCTime;
        void initTimer();

};

#endif