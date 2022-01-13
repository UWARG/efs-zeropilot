#pragma once

#include "attitudeStateManager.hpp"
#include "attitudeManager.hpp"
#include "AttitudeDatatypes.hpp"
#include "attitudeConfig.hpp"
#include "../../Common/Inc/AttitudePathInterface.hpp"
#include "../../Common/Inc/CommWithPathManager.hpp"

#include "SensorFusion.hpp"
#include "OutputMixing.hpp"
#include "PID.hpp"
#include "PWM.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

class pwmSetup : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr);
        static attitudeState& getInstance();
};

class fetchInstructionsMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        // TODO: fix GetPMInstructions to send data over interchip
        static CommandsForAM *GetPMInstructions(void) {return &_PMInstructions;}
        static CommandsForAM *GetTeleopInstructions(void) {return &_TeleopInstructions;}
        static bool isAutonomous(void) {return _isAutonomous;}
    private:
        fetchInstructionsMode() {
            //CommFromAMToPMInit();
            }
        fetchInstructionsMode(const fetchInstructionsMode& other);
        fetchInstructionsMode& operator =(const fetchInstructionsMode& other);
        static bool *ReceiveTeleopInstructions(void);
        static CommandsForAM _PMInstructions;
        static PPM_Instructions_t _TeleopInstructions;
        static bool _isAutonomous;
        static uint8_t teleopTimeoutCount;
        static uint8_t PMTimeoutCount;
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
