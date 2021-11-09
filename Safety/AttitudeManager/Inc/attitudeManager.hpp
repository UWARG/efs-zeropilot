#pragma once
#include "attitudeStateManager.hpp"

class attitudeState;

namespace AttMan
{
// Gives status of attitude manager so we know when it has completed a cycle (its state is FetchInstructionsMode) or entered failure mode
enum _Attitude_Manager_Cycle_Status {COMPLETED_CYCLE = 0, IN_CYCLE, FAILURE_MODE};
}

class attitudeManager
{
    public:
        attitudeManager();
        inline attitudeState* getCurrentState() const {return currentState;}
        void execute();
        void setState(attitudeState& newState);
        AttMan::_Attitude_Manager_Cycle_Status getStatus() {return status;}
    private:
        attitudeState* currentState;
        AttMan::_Attitude_Manager_Cycle_Status status;
};
