#pragma once
#include "attitudeStateManager.hpp"

class attitudeState;

// Gives status of attitude manager so we know when it has completed a cycle (its state is FetchInstructionsMode) or entered failure mode
enum _Attitude_Manager_Cycle_Status {COMPLETED_CYCLE = 0, IN_CYCLE, FAILURE_MODE};

class attitudeManager
{
    public:
        attitudeManager();
        inline attitudeState* getCurrentState() const {return currentState;}
        void execute();
        void setState(attitudeState& newState);
        _Attitude_Manager_Cycle_Status getStatus() {return status;}
    private:
        attitudeState* currentState;
        _Attitude_Manager_Cycle_Status status;
};
