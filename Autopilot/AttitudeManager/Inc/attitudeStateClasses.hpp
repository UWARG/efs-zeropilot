#pragma once

#include "attitudeStateManager.hpp"
#include "attitudeManager.hpp"
#include "AttitudeDatatypes.hpp"
#include "attitudeConfig.hpp"

#include "CommWithPathManager.hpp"
#include "SensorFusion.hpp"
#include "OutputMixing.hpp"
#include "PID.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

class decisionModuleMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;};
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;};
        static attitudeState& getInstance() { return &autonomous };
        static bool getAutonomousMode();
        // any other required functions (for Tony)

    private:
        bool autonomous; // boolean flag indicating if the drone is in autonomous mode (1) or teleop (0)
    
};

class fetchInstructionsMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static CommandsForAM *GetPMInstructions(void) {return &_MovementInstructions;} // get data from Path Manager over interchip
        static CommandsForAM *GetTeleopInstructions(void) {return &_MovementInstructions;} // get data from RC transmitter through teleop control
    private:
        fetchInstructionsMode() {CommFromAMToPMInit();}
        fetchInstructionsMode(const fetchInstructionsMode& other);
        fetchInstructionsMode& operator =(const fetchInstructionsMode& other);
        static CommandsForAM _MovementInstructions;
};

class sensorFusionMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static SFOutput_t *GetSFOutput(void) {return &_SFOutput;}
    private:
        sensorFusionMode() {SF_Init();}
        sensorFusionMode(const sensorFusionMode& other);
        sensorFusionMode& operator =(const sensorFusionMode& other);
        static SFOutput_t _SFOutput;
};

class PIDloopMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static PID_Output_t *GetPidOutput(void) {return &_PidOutput;}
    private:
        PIDloopMode() {}
        PIDloopMode(const PIDloopMode& other);
        PIDloopMode& operator =(const PIDloopMode& other);
        PIDController _rollPid{ROLL_PID_KP, ROLL_PID_KI, ROLL_PID_KD, ROLL_PID_INTEGRAL_MAX, ROLL_PID_MIN, ROLL_PID_MAX}; // Proportional, integral, derivative, min, max
        PIDController _pitchPid{PITCH_PID_KP, PITCH_PID_KI, PITCH_PID_KD, PITCH_PID_INTEGRAL_MAX, PITCH_PID_MIN, PITCH_PID_MAX};
        PIDController _yawPid{YAW_PID_KP, YAW_PID_KI, YAW_PID_KD, YAW_PID_INTEGRAL_MAX, YAW_PID_MIN, YAW_PID_MAX};
        PIDController _airspeedPid{AIRSPEED_PID_KP, AIRSPEED_PID_KI, AIRSPEED_PID_KD, AIRSPEED_PID_INTEGRAL_MAX, AIRSPEED_PID_MIN, AIRSPEED_PID_MAX};
        static PID_Output_t _PidOutput;
};

class OutputMixingMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static float *GetChannelOut(void) {return _channelOut;}
    private:
        OutputMixingMode() {}
        OutputMixingMode(const OutputMixingMode& other);
        OutputMixingMode& operator =(const OutputMixingMode& other);
        static float _channelOut[4];
};

class sendToSafetyMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        sendToSafetyMode() {SendToSafety_Init();} // Calls C-style initialization function
        sendToSafetyMode(const sendToSafetyMode& other);
        sendToSafetyMode& operator =(const sendToSafetyMode& other);
};

class FatalFailureMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
    private:
        FatalFailureMode() {}
        FatalFailureMode(const FatalFailureMode& other);
        FatalFailureMode& operator =(const FatalFailureMode& other);
};
