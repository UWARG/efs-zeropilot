#ifndef CRUISING_MODE_STAGES_HPP
#define CRUISING_MODE_STAGES_HPP

#include <cstdint>

#include "cruisingModeStageManager.hpp"
#include "cruisingMode.hpp"

#include "waypointManager.hpp"
#include "TelemPathInterface.hpp"
#include "AutoSteer.hpp"
#include "cruisingFlight.hpp"

class CruisingFlight : public CruisingModeStageManager {
    public:
        void enter(CruisingMode* cruise_mode) { (void) cruise_mode; }
        void execute(CruisingMode* cruise_mode);
        void exit(CruisingMode* cruise_mode) { (void) cruise_mode; }
        static CruisingModeStageManager& getInstance();

    private: 
        CruisingFlight() : in_hold {false}, going_home {false} {}
        CruisingFlight(const CruisingFlight& other);
        CruisingFlight& operator=(const CruisingFlight& other);

        WaypointManager cruising_state_manager;
        uint16_t waypoint_id_array[PATH_BUFFER_SIZE]; // Stores ids of the waypoints in the flight path in the order that they are executed
        static _WaypointManager_Data_In _input_data;
        static _WaypointManager_Data_Out _output_data;
        static _CruisingState_Telemetry_Return _return_to_ground;
        bool in_hold;
        bool going_home;
};

#endif

