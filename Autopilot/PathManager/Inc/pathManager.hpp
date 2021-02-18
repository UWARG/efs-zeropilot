// #pragma once is used to ensure that you don't include the same header file more than once?
#pragma once
// including the pure abstract class that we will define later on
#include "pathManagerStateManager.hpp"

//this is the pure abstract class... but why are we just writing the name of the class here?? !QUESTION FLAG!
class pathManagerState;

// Gives status of Path Manager so we know when it has completed a cycle (its state is communication with XYZ sensor state) or entered failure mode
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
    private:
        pathManagerState* currentState;
        _Path_Manager_Cycle_Status status;
};


