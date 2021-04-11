#pragma once

#include "pathManagerStateManager.hpp"
#include "cruisingState.hpp"
#include "pathManager.hpp"
#include "altimeter.hpp"
#include "gps.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"
#include "pathDataTypes.hpp"
#include "CommWithAttitudeManager.hpp"

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
        commsWithAttitude() { CommWithAMInit(); } // Initializes module
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
        commsWithTelemetry() {}
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
        static SFOutput_t* GetSFOutput(void) { return &sfOutputData; }
    private:
        sensorFusion() {}
        sensorFusion(const sensorFusion& other);
        sensorFusion& operator =(const sensorFusion& other);
        static SFOutput_t sfOutputData;
};

class cruisingState : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static _WaypointManager_Data_Out* GetOutputData(void) {return &_outputdata;}
        static _CruisingState_Telemetry_Return* GetErrorCodes(void) {return &_returnToGround;}

        #ifdef UNIT_TESTING
            WaypointManager* GetWaypointManager(void) {return &cruisingStateManager;}
            int* GetWaypointIdArray(void) {return waypointIDArray;}
        #endif
    private:
        cruisingState() {}
        cruisingState(const cruisingState& other);
        cruisingState& operator =(const cruisingState& other);

        WaypointManager cruisingStateManager;
        int waypointIDArray[PATH_BUFFER_SIZE]; // Stores ids of the waypoints in the flight path in the order that they are executed
        static _WaypointManager_Data_In _inputdata;
        static _WaypointManager_Data_Out _outputdata; 
        static _CruisingState_Telemetry_Return _returnToGround;
        bool inHold = false;
        bool goingHome = false;
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
