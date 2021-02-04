#pragma once

#include "attitudeStateManager.hpp"
#include "attitudeManager.hpp"
#include "AttitudeDatatypes.hpp"

#include "GetFromPathManager.hpp"
#include "SensorFusion.hpp"
#include "OutputMixing.hpp"
#include "PID.hpp"
#include "SendInstructionsToSafety.hpp"
#include "IMU.hpp"
#include "airspeed.hpp"
#include "fetchSensorMeasurementsMode.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#ifdef SIMULATION

#define IMU_CLASS SimulatedIMU
#define AIRSPEED_CLASS SimulatedAirspeed

#elif defined(UNIT_TESTING)

#define IMU_CLASS MockIMU
#define AIRSPEED_CLASS MockAirspeed

#else

#define IMU_CLASS ICM20602  // TODO to be replaced with the real classes once the sensor drivers are built
#define AIRSPEED_CLASS dummyairspeed

#endif
/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/


class fetchInstructionsMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static PMCommands *GetPMInstructions(void) {return &_PMInstructions;}
    private:
        fetchInstructionsMode() {}
        fetchInstructionsMode(const fetchInstructionsMode& other);
        fetchInstructionsMode& operator =(const fetchInstructionsMode& other);
        static PMCommands _PMInstructions;
};

class fetchSensorMeasurementsMode : public attitudeState
{
    public:
        void enter(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        void execute(attitudeManager* attitudeMgr);
        void exit(attitudeManager* attitudeMgr) {(void) attitudeMgr;}
        static attitudeState& getInstance();
        static IMU_Data_t *GetIMUOutput(void) {return &_imudata;}
        static Airspeed_Data_t *GetAirspeedOutput(void) {return &_airspeeddata;}
    private:
        fetchSensorMeasurementsMode() {}
        fetchSensorMeasurementsMode(const fetchSensorMeasurementsMode& other);
        fetchSensorMeasurementsMode& operator =(const fetchSensorMeasurementsMode& other);
        static IMU_Data_t _imudata;
        static Airspeed_Data_t _airspeeddata;
        IMU_CLASS ImuSens;
        AIRSPEED_CLASS AirspeedSens;
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
        sensorFusionMode() {}
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
        PIDController _rollPid{1, 0, 0, 0, -100, 100};
        PIDController _pitchPid{1, 0, 0, 0, -100, 100};
        PIDController _yawPid{1, 0, 0, 0, -100, 100};
        PIDController _airspeedPid{1, 0, 0, 0, 0, 100};
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
