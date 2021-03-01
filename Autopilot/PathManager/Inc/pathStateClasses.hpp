#pragma once

#include "pathManagerStateManager.hpp"
#include "pathManager.hpp"
#include "altimeter.hpp"
#include "gps.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"

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

class getFromTelemetry : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
    private:
        getFromTelemetry() {}
        getFromTelemetry(const getFromTelemetry& other);
        getFromTelemetry& operator =(const getFromTelemetry& other);
};

class getSensorData : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static AltimeterData_t *GetAltimeterOutput(void) {return &_altimeterdata;}
        static GpsData_t *GetGPSOutput(void) {return &_gpsdata;}
    private:
        getSensorData() {}
        getSensorData(const getSensorData& other);
        getSensorData& operator =(const getSensorData& other);
        static AltimeterData_t _altimeterdata; 
        static GpsData_t _gpsdata; 
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
        static _PathData *GetWaypointData(void) {return &_waypointdata;}
        static _WaypointManager_Data_Out *GetOutputData(void) {return &_outputdata;}
    private:
        cruisingState() {}
        cruisingState(const cruisingState& other);
        cruisingState& operator =(const cruisingState& other);
        //WaypointManager(0,0) {} initializing instance of WaypointManager class
        int waypointIDArray[PATH_BUFFER_SIZE]; 
        static _PathData _waypointdata; 
        static _WaypointManager_Data_Out _outputdata; 
};

class coordinateTurnElevation : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static CoordinatedTurnAttitudeManagerCommands_t *GetRollAndRudder(void) {return &_rollandrudder;}
        static AltitudeAirspeedCommands_t *GetPitchAndAirspeed(void) {return &_pitchandairspeed;}
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



