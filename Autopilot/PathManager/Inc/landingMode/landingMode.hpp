#ifndef LANDING_MODE_HPP
#define LANDING_MODE_HPP

#include "pathMode.hpp"
#include "landingModeStageManager.hpp"
#include "pathModeSelector.hpp"
#include "SensorFusion.hpp"

class LandingModeStageManager;

enum LandingStages {
    LANDING_TRANSITION = 0,
    LANDING_SLOPE,
    LANDING_FLARE,
    LANDING_DECRAB,
    LANDING_TOUCHDOWN
};

class LandingMode : public PathMode {
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
        inline LandingModeStageManager* getCurrentStage() const { return current_stage; }
        
        /**
         * Sets the pointer for the current stage of flight
         * 
         * @param new_stage -> new stage of flight within mode
         * 
         * @return none
         */
        inline void setCurrentStage(LandingModeStageManager* new_stage) { current_stage = new_stage; }

        /**
         * Returns a pointer for the current stage of flight
         * 
         * @return current stage of flight within mode
         */
        inline LandingStages getCurrentStageEnum() const { return landing_stage; }

        /**
         * Sets the enum for the current stage of flight
         * 
         * @param new_stage -> enum for new stage of flight within mode
         * 
         * @return none
         */
        inline void setCurrentStageEnum(LandingStages new_stage) { landing_stage = new_stage; }

        /*
         * Getters and setters for Landing Mode data
         */
        inline bool getMadeLandingPoints() { return made_landing_points; }

        inline void setMadeLandingPoints(bool new_made_landing_points) { made_landing_points = new_made_landing_points; }

        inline bool getIsPackage() { return is_package; }

        inline void setIsPackage(bool new_is_package) { is_package = new_is_package; }

    private: 
        LandingMode();

        LandingModeStageManager* current_stage;
        LandingStages landing_stage;
        static PathModeSelector* _mode_selector;  

        bool made_landing_points;
        bool is_package;
};

#endif


