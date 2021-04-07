#pragma once

#include "pathManagerStateManager.hpp"
#include "cruisingState.hpp"
#include "pathManager.hpp"
#include "AutoSteer.hpp"
#include "waypointManager.hpp"
#include "landingTakeoffManager.hpp"
#include "pathDatatypes.hpp"
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

        //resetPassby uses operator chaining, for more info, refer to the assignment operator chaining in the following link http://courses.cms.caltech.edu/cs11/material/cpp/donnie/cpp-ops.html
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
        static _PathData takeoffPoint;
        static _WaypointStatus waypointStatus;

    private:
        static _PathData * pathArray[1];
        static _PathData * currentLocation;
        
        
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

class landingTransitionStage : public pathManagerState
{
    public:
        void enter(pathManager* pathMgr) {(void) pathMgr;}
        void execute(pathManager* pathMgr);
        void exit(pathManager* pathMgr) {(void) pathMgr;}
        static pathManagerState& getInstance();

        static WaypointManager landingPath;
        int waypointIDArray[PATH_BUFFER_SIZE];
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


