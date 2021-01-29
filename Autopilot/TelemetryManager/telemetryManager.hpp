#pragma once
#include "telemetryStateManager.hpp"

class telemetryState;

class telemetryManager
{
    public:
        telemetryManager();
        inline telemetryState* getCurrentState() const{return currentState;}
        void execute();
        void setState(telemetryState& newState);
        bool dataValid;
        bool dataError;
    private:
        telemetryState* currentState;
};