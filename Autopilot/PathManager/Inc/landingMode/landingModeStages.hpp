#ifndef LANDING_MODE_STAGES_HPP
#define LANDING_MODE_STAGES_HPP

#include "landingModeStageManager.hpp"
#include "landingMode.hpp"

#include "waypointManager.hpp"
#include "AutoSteer.hpp"
#include "TelemPathInterface.hpp"
#include "landingTakeoffManager.hpp"

class LandingTransitionStage : public LandingModeStageManager {
    public:
        void enter(LandingMode* land_mode) {(void) land_mode;}
        void execute(LandingMode* land_mode);
        void exit(LandingMode* land_mode) {(void) land_mode;}
        static LandingModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}

        static WaypointManager _landing_path;
        static _WaypointStatus _waypoint_status; //used to catch errors
        static _LandingPath _path; //used to load in path

    private:
        int waypoint_id_array[PATH_BUFFER_SIZE];
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_In _waypoint_input;
        static _WaypointManager_Data_Out _waypoint_output;
        static _PathData* _path_array[3]; //used to translate loaded in path to something the waypoint manager can take as a parameter
        static _PathData* _current_location;

        LandingTransitionStage() {}
        LandingTransitionStage(const LandingTransitionStage& other);
        LandingTransitionStage& operator =(const LandingTransitionStage& other);
};

class LandingSlopeStage : public LandingModeStageManager {
    public:
        void enter(LandingMode* land_mode) {(void) land_mode;}
        void execute(LandingMode* land_mode);
        void exit(LandingMode* land_mode) {(void) land_mode;}
        static LandingModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}
        
    private:
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_In _waypoint_input;
        static _WaypointManager_Data_Out _waypoint_output;

        LandingSlopeStage() {}
        LandingSlopeStage(const LandingSlopeStage& other);
        LandingSlopeStage& operator =(const LandingSlopeStage& other);
};

class LandingFlareStage : public LandingModeStageManager {
    public:
        void enter(LandingMode* land_mode) {(void) land_mode;}
        void execute(LandingMode* land_mode);
        void exit(LandingMode* land_mode) {(void) land_mode;}
        static LandingModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}

    private:
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_In _waypoint_input;
        static _WaypointManager_Data_Out _waypoint_output;

        LandingFlareStage() {}
        LandingFlareStage(const LandingFlareStage& other);
        LandingFlareStage& operator =(const LandingFlareStage& other);
};

class LandingDecrabStage : public LandingModeStageManager {
    public:
        void enter(LandingMode* land_mode) {(void) land_mode;}
        void execute(LandingMode* land_mode);
        void exit(LandingMode* land_mode) {(void) land_mode;}
        static LandingModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}

    private:
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_Out _waypoint_output;

        LandingDecrabStage() {}
        LandingDecrabStage(const LandingDecrabStage& other);
        LandingDecrabStage& operator =(const LandingDecrabStage& other);
};

class LandingTouchdownStage : public LandingModeStageManager {
    public:
        void enter(LandingMode* land_mode) {(void) land_mode;}
        void execute(LandingMode* land_mode);
        void exit(LandingMode* land_mode) {(void) land_mode;}
        static LandingModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}

    private:
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_Out _waypoint_output;

        LandingTouchdownStage() {}
        LandingTouchdownStage(const LandingTouchdownStage& other);
        LandingTouchdownStage& operator =(const LandingTouchdownStage& other);
};

#endif


