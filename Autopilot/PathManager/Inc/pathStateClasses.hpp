#pragma once

#include "pathManagerStateManager.hpp"
#include "pathManager.hpp"
#include "altimeter.hpp"
#include "gps.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"
#include "landingTakeoffManager.hpp"
#include "pathDatatypes.hpp"

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
        static Telemetry_PIGO_t telemetryInput;
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
        static _WaypointManager_Data_In sensorInput; //must be filled out
    private:
        sensorFusion() {}
        sensorFusion(const sensorFusion& other);
        sensorFusion& operator =(const sensorFusion& other);
};

class resetVariables : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
    private:
        resetVariables() {}
        resetVariables(const resetVariables& other);
        resetVariables& operator =(const resetVariables& other);
        void resetPassby(_PassbyControl* controlDetails){controlDetails->rollPassby = controlDetails->pitchPassby = controlDetails->throttlePassby = controlDetails->rudderPassby = false;}
};

class takeoffRollStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static WaypointManager takeoffPath;
        static _PathData * pathArray[1];
        static _PathData * currentLocation;
        static _WaypointStatus waypointStatus;
        static _PathData takeoffPoint;
    private:
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
    private:
        takeoffClimbStage() {}
        takeoffClimbStage(const takeoffClimbStage& other);
        takeoffClimbStage& operator =(const takeoffClimbStage& other);
};


class cruisingState : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();
        static _PathData *GetWaypointData(void) {return &_waypointdata;}
        static _WaypointManager_Data_Out _outputdata;
    private:
        cruisingState() {}
        cruisingState(const cruisingState& other);
        cruisingState& operator =(const cruisingState& other);
        //WaypointManager(0,0) {} initializing instance of WaypointManager class
        int waypointIDArray[PATH_BUFFER_SIZE]; 
        static _PathData _waypointdata; 
         
};

class landingTransitionStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

        static WaypointManager landingPath; //in future merge, parameters needs to be taken out *****
        static _LandingPath path; //used to load in path
        static _PathData * pathArray[3]; //used to translate loaded in path to something the waypoint manager can take as a parameter
        static _PathData * currentLocation;
        static _WaypointStatus waypointStatus; //used to catch errors
        static double differenceInHeading1;
        static double differenceInHeading2;

    private:
        landingTransitionStage() {}
        landingTransitionStage(const landingTransitionStage& other);
        landingTransitionStage& operator =(const landingTransitionStage& other);

};

class landingSlopeStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

    private:
        landingSlopeStage() {}
        landingSlopeStage(const landingSlopeStage& other);
        landingSlopeStage& operator =(const landingSlopeStage& other);
};

class landingFlareStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

    private:
        landingFlareStage() {}
        landingFlareStage(const landingFlareStage& other);
        landingFlareStage& operator =(const landingFlareStage& other);
};

class landingDecrabStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

    private:
        landingDecrabStage() {}
        landingDecrabStage(const landingDecrabStage& other);
        landingDecrabStage& operator =(const landingDecrabStage& other);
};

class landingTouchdownStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

    private:
        landingTouchdownStage() {}
        landingTouchdownStage(const landingTouchdownStage& other);
        landingTouchdownStage& operator =(const landingTouchdownStage& other);
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


