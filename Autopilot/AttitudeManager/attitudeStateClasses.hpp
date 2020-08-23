#pragma once
#include "attitudeStateManager.hpp"
#include "attitudeManager.hpp"

class sensorInputMode : public attitudeState
{
    public: 
        void enter(attitudeManager* attitudeMgr) {}
        void toggle(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {}
        static attitudeState& getInstance();
    private:
        sensorInputMode() {}
        sensorInputMode(const sensorInputMode& other);
        sensorInputMode& operator =(const sensorInputMode& other);
};

class PIDloopMode : public attitudeState
{
    public: 
        void enter(attitudeManager* attitudeMgr) {}
        void toggle(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {}
        static attitudeState& getInstance();
    private:
        PIDloopMode() {}
        PIDloopMode(const PIDloopMode& other);
        PIDloopMode& operator =(const PIDloopMode& other);
};

class sendToSafetyMode : public attitudeState
{
    public: 
        void enter(attitudeManager* attitudeMgr) {}
        void toggle(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {}
        static attitudeState& getInstance();
    private:
        sendToSafetyMode() {}
        sendToSafetyMode(const sendToSafetyMode& other);
        sendToSafetyMode& operator =(const sendToSafetyMode& other);
};

//Reminder: add failed state mode?
