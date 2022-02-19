/*
* Author: Nixon Chan
*/

#ifndef TELEMETRYSTATEMANAGER_HPP
#define TELEMETRYSTATEMANAGER_HPP

#include "telemetryManager.hpp"
// #include "UARTDriver.hpp"
// #include"comms.hpp"
// #include"FW_CV_Structs.hpp"
// #include "CommsWithPathManager.hpp"

class telemetryManager;

class telemetryState
{
    public: 
        virtual void enter(telemetryManager* telemetryMgr) = 0;
        virtual void execute(telemetryManager* telemetryMgr) = 0;
        virtual void exit(telemetryManager* telemetryMgr) = 0;

        bool operator==(const telemetryState& rhs) const{return(this == &rhs);}

        virtual ~telemetryState(){}  
};

#endif

