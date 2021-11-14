#ifndef TAKEOFF_MODE_STAGES_HPP
#define TAKEOFF_MODE_STAGES_HPP

#include "takeoffModeStageManager.hpp"
#include "takeoffMode.hpp"

#include "waypointManager.hpp"
#include "AutoSteer.hpp"
#include "TelemPathInterface.hpp"
#include "landingTakeoffManager.hpp"

class TakeoffRollStage : public TakeoffModeStageManager {
    public:
        void enter(TakeoffMode* takeoff_mode) {(void) takeoff_mode;}
        void execute(TakeoffMode* takeoff_mode);
        void exit(TakeoffMode* takeoff_mode) {(void) takeoff_mode;}
        static TakeoffModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}

        static WaypointManager _takeoff_path;
        static _PathData _takeoff_point;
        static _WaypointStatus _waypoint_status;

    private:
        static _PathData * _path_array[1];
        static _PathData * _current_location;
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_Out _waypoint_output;

        TakeoffRollStage() {}
        TakeoffRollStage(const TakeoffRollStage& other);
        TakeoffRollStage& operator =(const TakeoffRollStage& other);
};

class TakeoffClimbStage : public TakeoffModeStageManager {
    public:
        void enter(TakeoffMode* takeoff_mode) {(void) takeoff_mode;}
        void execute(TakeoffMode* takeoff_mode);
        void exit(TakeoffMode* takeoff_mode) {(void) takeoff_mode;}
        static TakeoffModeStageManager& getInstance();

        static _LandingTakeoffOutput* getControlOutput(){return &_output;}
        
    private:
        static _LandingTakeoffInput _input;
        static _LandingTakeoffOutput _output;
        static _WaypointManager_Data_In _waypoint_input;
        static _WaypointManager_Data_Out _waypoint_output;

        TakeoffClimbStage() {}
        TakeoffClimbStage(const TakeoffClimbStage& other);
        TakeoffClimbStage& operator =(const TakeoffClimbStage& other);
};


#endif


