#pragma once

#include "pathManagerStateManager.hpp"
#include "cruisingState.hpp"
#include "pathManager.hpp"
#include "altimeter.hpp"
#include "gps.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"
#include "pathDataTypes.hpp"
#include "getSensordata.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#ifdef SIMULATION

#define GPS_CLASS SimulatedGPS
#define ALTIMETER_CLASS SimulatedAltimeter

#elif defined(UNIT_TESTING)

#define GPS_CLASS MockGPS
#define ALTIMETER_CLASS MockAltimeter

#else

#define GPS_CLASS NEOM8  
#define ALTIMETER_CLASS MS5637

#endif

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
    private:
        commsWithAttitude() {}
        commsWithAttitude(const commsWithAttitude& other);
        commsWithAttitude& operator =(const commsWithAttitude& other);
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

class getSensorData : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static Altimeter_Data_t* GetAltimeterOutput(void) {return &_altimeterdata;}
        static Gps_Data_t* GetGPSOutput(void) {return &_gpsdata;}
    private:
        getSensorData() {}
        getSensorData(const getSensorData& other);
        getSensorData& operator =(const getSensorData& other);
        static Altimeter_Data_t _altimeterdata; 
        static Gps_Data_t _gpsdata; 
        GPS_CLASS GpsSens;
        ALTIMETER_CLASS AltimeterSens;
};

class sensorFusion : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
    private:
        sensorFusion() {}
        sensorFusion(const sensorFusion& other);
        sensorFusion& operator =(const sensorFusion& other);
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
        coordinateTurnElevation() {}
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
