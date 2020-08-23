#pragma once
#include "attitudeStateManager.hpp"

class attitudeState;

class attitudeManager
{
    public:
        attitudeManager();
        inline attitudeState* getCurrentState() const {return currentState;}
        void toggle();
        void setState(attitudeState& newState);

    private:
        attitudeState* currentState;
};
