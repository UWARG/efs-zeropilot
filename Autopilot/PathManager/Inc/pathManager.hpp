#pragma once
#include "pathManagerStateManager.hpp"
#include "pathDatatypes.hpp"

class pathManagerState;

namespace PathMan
{

// Gives status of Path Manager so we know when it has completed a cycle, or entered failure mode.
enum _Path_Manager_Cycle_Status {COMPLETED_CYCLE = 0, IN_CYCLE, FAILURE_MODE};

}

class pathManager
{
    public:
        pathManager();
        inline pathManagerState* getCurrentState() const {return currentState;}
        void execute();
        void setState(pathManagerState& newState);
        PathMan::_Path_Manager_Cycle_Status getStatus() {return status;}

        //used to determine the stage of the landing sequence
        _FlightStage stage;
        bool madeLandingPoints;
        bool madeTakeoffPoints;
        bool isError;
        bool isPackage;
    private:
        pathManagerState* currentState;
        PathMan::_Path_Manager_Cycle_Status status;
};


