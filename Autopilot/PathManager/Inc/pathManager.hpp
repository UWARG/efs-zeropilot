#pragma once
#include "pathManagerStateManager.hpp"

class pathManagerState;

// Gives status of Path Manager so we know when it has completed a cycle, or entered failure mode. 
// enum values are assigned as consecutive numbers based on the first value assigned. 
enum _Path_Manager_Cycle_Status {COMPLETED_CYCLE = 0, IN_CYCLE, FAILURE_MODE};

class pathManager
{
    public:
        pathManager();
        inline pathManagerState* getCurrentState() const {return currentState;}
        void execute();
        void setState(pathManagerState& newState);
        _Path_Manager_Cycle_Status getStatus() {return status;}

        _LandingStage getLandingStage(){return stage;}
        void setLandingStage(_LandingStage changingStage) {stage = changingStage;}
    private:
        pathManagerState* currentState;
        _Path_Manager_Cycle_Status status;

        //used to determine the stage of the landing sequence
        _LandingStage stage;
};


