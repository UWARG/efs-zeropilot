#ifndef TAKEOFF_MODE_HPP
#define TAKEOFF_MODE_HPP

#include "pathMode.hpp"
#include "takeoffModeStageManager.hpp"
#include "pathModeSelector.hpp"
#include "SensorFusion.hpp"

class TakeoffModeStageManager;

enum TakeoffStages {
    TAKEOFF_ROLL = 0,
    TAKEOFF_CLIMB
};

class TakeoffMode : public PathMode {
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
        inline TakeoffModeStageManager* getCurrentStage() const { return current_stage; }
        
        /**
         * Sets the pointer for the current stage of flight
         * 
         * @param new_stage -> new stage of flight within mode
         * 
         * @return none
         */
        inline void setCurrentStage(TakeoffModeStageManager* new_stage) { current_stage = new_stage; }

        /**
         * Returns a pointer for the current stage of flight
         * 
         * @return current stage of flight within mode
         */
        inline TakeoffStages getCurrentStageEnum() const { return takeoff_stage; }

        /**
         * Sets the enum for the current stage of flight
         * 
         * @param new_stage -> enum for new stage of flight within mode
         * 
         * @return none
         */
        inline void setCurrentStageEnum(TakeoffStages new_stage) { takeoff_stage = new_stage; }

        /*
         * Getters and setters for takeoff Mode data
         */
        inline bool getMadeTakeoffPoints() { return made_takeoff_points; }

        inline void setMadeTakeoffPoints(bool new_made_takeoff_points) { made_takeoff_points = new_made_takeoff_points; }

        inline bool getIsPackage() { return is_package; }

        inline void setIsPackage(bool new_is_package) { is_package = new_is_package; }

    private: 
        TakeoffMode();

        TakeoffModeStageManager* current_stage;
        TakeoffStages takeoff_stage;
        static PathModeSelector* _mode_selector;  

        bool made_takeoff_points;
        bool is_package;
};

#endif

