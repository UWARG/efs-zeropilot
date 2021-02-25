#pragma once
#include "pathManagerStateManager.hpp"
#include "landingManager.hpp"

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

        //used to set landing enum
        void setLandingStage(_LandingStage desiredStage){currentLandingStage = desiredStage;}
        _LandingStage getLandingStage() {return currentLandingStage;}

        _Path_Manager_Cycle_Status getStatus() {return status;}
    private:
        pathManagerState* currentState;

        //used to determine stage of landing
        _LandingStage currentLandingStage;
        
        _Path_Manager_Cycle_Status status;
};


