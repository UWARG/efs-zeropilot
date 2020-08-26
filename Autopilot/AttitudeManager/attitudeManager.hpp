#pragma once
#include "attitudeStateManager.hpp"

class attitudeState;

class attitudeManager
{
    public:
        attitudeManager();
        inline attitudeState* getCurrentState() const {return currentState;}
        void execute();
        void setState(attitudeState& newState);

    private:
        attitudeState* currentState;
};
