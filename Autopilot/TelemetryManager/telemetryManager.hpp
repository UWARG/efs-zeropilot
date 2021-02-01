#pragma once
#include "telemetryStateManager.hpp"

class telemetryState;

//status of the manager
enum _Telemetry_Manager_Cycle_Status {COMPLETED_CYCLE=0, IN_CYCLE, FAILURE_MODE};

class telemetryManager
{
    public:
        telemetryManager();
        inline telemetryState* getCurrentState() const{return currentState;}
        void execute();
        void setState(telemetryState& newState);
        bool dataValid;
        bool dataError;
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryManager.hpp
    private:
        telemetryState* currentState;
};
=======
        bool regularReport; //for convinience
        bool fatalFail = false; //any point in the states, set this variable to transition to failed state
        int cycleCounter = 0;
        const int CLK_TIME_CONSTANT = 1;
        _Telemetry_Manager_Cycle_Status getStatus() {return status;}
    private:
        telemetryState* currentState; //state of the manager
        _Telemetry_Manager_Cycle_Status status;
};
>>>>>>> Stashed changes:TelemetryManager/telemetryManager.hpp
