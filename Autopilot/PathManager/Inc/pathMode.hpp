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
         * @param telemetry_in -> telemetry data from ground 
         * @param sensor_fusion_in -> sensor fusion data
         * @param imu_data_in -> raw imu data
         * 
         * @return none
         */
        virtual void execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) = 0;

        /**
         * Get telemetry data that was sent in by path manager
         *
         * @return telemetry data sent by path manager
         */
        Telemetry_PIGO_t getTelemetryData() { return telemetry_data; }

        /**
         * Get sensor fusion data that was sent in by path manager
         *
         * @return sensor fusion data sent by path manager
         */
        SFOutput_t getSensorFusionData() { return sf_data; }

        /**
         * Get imu data that was sent in by path manager
         *
         * @return imu data sent by path manager
         */
        IMU_Data_t getImuData() { return imu_data; }

        bool operator==(const PathMode& rhs) const {return (this == &rhs);} 

        /**
         * Resets control details for landing and takeoff stages
         * 
         * ResetPassby uses operator chaining, for more info, refer to the assignment operator chaining in the following link http://courses.cms.caltech.edu/cs11/material/cpp/donnie/cpp-ops.html
         *
         * @param control_details -> control details for current stage
         * 
         * @return none
         */
        void resetPassby(_PassbyControl* control_details){control_details->rollPassby = control_details->pitchPassby = control_details->throttlePassby = control_details->rudderPassby = false;}
        
    protected:
        PathMan::_Path_Manager_Cycle_Status current_status;
        Telemetry_PIGO_t telemetry_data;
        SFOutput_t sf_data;
        IMU_Data_t imu_data;
};

#endif

