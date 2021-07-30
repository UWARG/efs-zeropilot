#pragma once

#include "pathManagerStateManager.hpp"
#include "pathModeSelector.hpp"
#include "pathManager.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"
#include "landingTakeoffManager.hpp"
#include "pathDatatypes.hpp"
#include "CommWithAttitudeManager.hpp"
#include "CommsWithTelemetry.hpp"
#include "SensorFusion.hpp"
#include "AttitudePathInterface.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

class commsWithAttitude : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static AttitudeData* GetCommWithAttitudeData(void) { return &_receivedData; }
    private:
        commsWithAttitude() { CommFromPMToAMInit(); } // Initializes module
        commsWithAttitude(const commsWithAttitude& other);
        commsWithAttitude& operator =(const commsWithAttitude& other);
        static AttitudeData _receivedData;
};

class commsWithTelemetry : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static Telemetry_PIGO_t* GetTelemetryIncomingData(void) {return &_incomingData;}
    private:
        commsWithTelemetry() {CommWithTelemInit();}
        commsWithTelemetry(const commsWithTelemetry& other);
        commsWithTelemetry& operator =(const commsWithTelemetry& other);
        static Telemetry_PIGO_t _incomingData; // Stores the commands sent by telemetry for easy access by other states in the pathmanager
};

class sensorFusion : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static SFOutput_t* GetSFOutput(void) { return &_sfOutputData; }
        static IMU_Data_t* GetIMUData(void) { return &_imudata; }
    private:
        sensorFusion() {}
        sensorFusion(const sensorFusion& other);
        sensorFusion& operator =(const sensorFusion& other);
        static SFOutput_t _sfOutputData;
        static IMU_Data_t _imudata;
        // If additional raw sensor data is needed, add the static struct here and implement
        // it like the IMU_Data_t struct above
};

class modeExecutor : public pathManagerState {
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

    private:
        modeExecutor() {} // Initializes module
        modeExecutor(const modeExecutor& other);
        modeExecutor& operator =(const modeExecutor& other);
};

class takeoffRollStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static WaypointManager takeoffPath;
        static _PathData takeoffPoint;
        static _WaypointStatus waypointStatus;
        static _LandingTakeoffOutput* getControlOutput(){return &output;}

    private:
        static _PathData * pathArray[1];
        static _PathData * currentLocation;
        static _LandingTakeoffInput input;
        static _LandingTakeoffOutput output;
        static _WaypointManager_Data_Out waypointOutput;
        takeoffRollStage() {}
        takeoffRollStage(const takeoffRollStage& other);
        takeoffRollStage& operator =(const takeoffRollStage& other);

};

class takeoffClimbStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static _LandingTakeoffOutput* getControlOutput(){return &output;}
    private:
        static _LandingTakeoffInput input;
        static _LandingTakeoffOutput output;
        static _WaypointManager_Data_In waypointInput;
        static _WaypointManager_Data_Out waypointOutput;
        takeoffClimbStage() {}
        takeoffClimbStage(const takeoffClimbStage& other);
        takeoffClimbStage& operator =(const takeoffClimbStage& other);
};

class coordinateTurnElevation : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static CoordinatedTurnAttitudeManagerCommands_t* GetRollAndRudder(void) {return &_rollandrudder;}
        static AltitudeAirspeedCommands_t* GetPitchAndAirspeed(void) {return &_pitchandairspeed;}
    private:
        coordinateTurnElevation() { AutoSteer_Init(); } // Initializes autosteer module
        coordinateTurnElevation(const coordinateTurnElevation& other);
        coordinateTurnElevation& operator =(const coordinateTurnElevation& other);
        static _LandingTakeoffOutput landingTakeoffOutput;
        static _WaypointManager_Data_Out cruisingOutput;
        static CoordinatedTurnInput_t turnInput;
        static AltitudeAirspeedInput_t altitudeAirspeedInput;
        static CoordinatedTurnAttitudeManagerCommands_t _rollandrudder;
        static AltitudeAirspeedCommands_t _pitchandairspeed;
};

class fatalFailureMode : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
    private:
        fatalFailureMode() {}
        fatalFailureMode(const fatalFailureMode& other);
        fatalFailureMode& operator =(const fatalFailureMode& other);
};


