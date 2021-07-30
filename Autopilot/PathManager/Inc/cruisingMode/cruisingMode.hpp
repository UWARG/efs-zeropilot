#ifndef CRUISING_MODE_HPP
#define CRUISING_MODE_HPP

#include "pathMode.hpp"
#include "cruisingModeStageManager.hpp"
#include "pathModeSelector.hpp"

class CruisingModeStageManager;

// Mode class as depicted in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class CruisingMode : public PathMode { 
    public:
        /**
         * Call to get singleton for this class
         *
         * @return instance of class object
         */
        static PathMode& getInstance();
        
        /**
         * Execute the curent stage in the mode
         * 
         * @param telemetry_in -> telemetry data from ground 
         * @param sensor_fusion_in -> sensor fusion data
         * @param imu_data_in -> raw imu data
         *
         * @return none
         */
        void execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in);
        
        /**
         * Get PathModeSelector singleton instance
         *
         * @return instance of PathModeSelector singleton
         */
        PathModeSelector* getModeSelector();

        /**
         * Returns a pointer for the current stage of flight
         * 
         * @return current stage of flight within mode
         */
        inline CruisingModeStageManager* getCurrentStage() const { return current_stage; }
        
        // No need a setStage method as we only have one stage
    private: 
        CruisingMode();

        CruisingModeStageManager* current_stage;
        static PathModeSelector* _mode_selector;        
};

#endif

