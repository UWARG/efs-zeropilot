#pragma once
#include "attitudeStateManager.hpp"
#include "attitudeManager.hpp"

class fetchInstructionsMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        fetchInstructionsMode() {}
        fetchInstructionsMode(const fetchInstructionsMode& other);
        fetchInstructionsMode& operator =(const fetchInstructionsMode& other);
};

class sensorFusionMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        sensorFusionMode() {}
        sensorFusionMode(const sensorFusionMode& other);
        sensorFusionMode& operator =(const sensorFusionMode& other);
};

class PIDloopMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        PIDloopMode() {}
        PIDloopMode(const PIDloopMode& other);
        PIDloopMode& operator =(const PIDloopMode& other);
};

class OutputMixingMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        OutputMixingMode() {}
        OutputMixingMode(const OutputMixingMode& other);
        OutputMixingMode& operator =(const OutputMixingMode& other);
};

class sendToSafetyMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        sendToSafetyMode() {}
        sendToSafetyMode(const sendToSafetyMode& other);
        sendToSafetyMode& operator =(const sendToSafetyMode& other);
};

//Reminder: add failed state mode?
