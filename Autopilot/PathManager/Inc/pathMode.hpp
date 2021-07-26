#ifndef PATH_MODE_HPP
#define PATH_MODE_HPP

#include "pathManager.hpp"
#include "TelemPathInterface.hpp"
#include "SensorFusion.hpp"

// Parent for the Mode class in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class PathMode {      
    public:
        /**
         * Execute method called by PathModeSelector::execute([:params]) to execute the current stage of our current mode of flight
         * 
         * @return none
         */
        virtual void execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in) = 0;

        Telemetry_PIGO_t getTelemetryData() { return telemetry_data; }
        SFOutput_t getSensorFusionData() { return sensor_fusion_output; }
        

    protected:
        PathMan::_Path_Manager_Cycle_Status current_status;
        Telemetry_PIGO_t telemetry_data;
        SFOutput_t sensor_fusion_output;
};

#endif