#ifndef PATH_MODE_SELECTOR_HPP
#define PATH_MODE_SELECTOR_HPP

#include "pathMode.hpp"
#include "TelemPathInterface.hpp"
#include "SensorFusion.hpp"
#include "AutoSteer.hpp"
#include "SensorFusion.hpp"
#include "pathDatatypes.hpp"

#ifdef UNIT_TESTING
    enum PathModeEnum {MODE_TAKEOFF = 0, MODE_CRUISING, MODE_LANDING, MODE_TAXIING, MODE_TESTING_NONE};
#else
    enum PathModeEnum {MODE_TAKEOFF = 0, MODE_CRUISING, MODE_LANDING, MODE_TAXIING};
#endif

// ModeSelector in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class PathModeSelector {
    public:
        /**
         * Call to get singleton for this clas
         *
         * @return instance of class object
         */
        static PathModeSelector* getInstance();

        /**
         * Called by the ModeExecutor state in PathManager to execute the correct mode of flight
         * 
         * @param telemetry_in -> telemetry data from ground 
         * @param sensor_fusion_in -> sensor fusion data
         * @param imu_data_in -> raw imu data
         *
         * @return none
         */
        void execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in);

        /**
         * Returns a pointer to the current mode of flight. Note that a PathMode object is returned, which is the
         * parent class to all Mode classes
         * 
         * @return pointer to the current PathMode object
         */
        PathMode* getCurrentMode() { return current_mode; }

        /**
         * Returns enum for the current mode of flight
         * 
         * @return enum representing current mode of flight
         */
        PathModeEnum getCurrentModeEnum() { return current_mode_enum; }

        /**
         * Set current_mode parameter
         * 
         * @param mode -> new mode of flight (PathMode object [parent object] used for polymorphism)
         *
         * @return none
         */
        void setCurrentMode(PathMode& newMode) { current_mode = &newMode; }
        
        /**
         * Set current_mode_enum parameter
         * 
         * @param newMode -> enum for new mode of flight
         *
         * @return none
         */
        void setCurrentModeEnum(PathModeEnum newMode) { current_mode_enum = newMode; }

        /**
         * Set the altitude_airspeed_input parameter, which will be used by coordinateTurnElevation 
         *
         * @param alt_airspeed_input -> new value
         * 
         * @return none
         */
        void setAltitudeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input);

        /**
         * get the altitude_airspeed_input parameter, which will be used by coordinateTurnElevation 
         * 
         * @return altitude_airspeed_input parameter
         */
        AltitudeAirspeedInput_t getAltitudeAirspeedInput() { return altitude_airspeed_input; }

        /**
         * Set the coordinated_turn_input parameter, which will be used by coordinateTurnElevation 
         *
         * @param coord_turn_input -> new value
         * 
         * @return none
         */
        void setCoordinatedTurnInput(CoordinatedTurnInput_t coord_turn_input);

        /**
         * get the coordinated_turn_input parameter, which will be used by coordinateTurnElevation 
         * 
         * @return coordinated_turn_input parameter
         */
        CoordinatedTurnInput_t getCoordinatedTurnInput() { return coordinated_turn_input; }

        /**
         * Set the coordinated_turn_input parameter, which will be used by coordinateTurnElevation 
         *
         * @param coord_turn_input -> new value
         * 
         * @return none
         */
        void setPassbyControl(_PassbyControl passby);

        /**
         * get the coordinated_turn_input parameter, which will be used by coordinateTurnElevation 
         * 
         * @return coordinated_turn_input parameter
         */
        _PassbyControl getPassbyControl() { return passby_control_output; }

        /**
         * set the is_error parameter, which will be used by modeExecutor to transition to our next state 
         * 
         * @param new_is_error -> new error value
         *
         * @return none
         */
        inline void setIsError(bool new_is_error) { is_error = new_is_error; }
        
        /**
         * get the is_error parameter, which will be used by modeExecutor to transition to our next state 
         * 
         * @return is_error parameter
         */
        inline bool getIsError() { return is_error; }

        /*
         * Getters and setters for Cruising Mode Telemetry Data
         */
        void setEditingFlightPathErrorCode(uint8_t new_val) { editing_flight_path_error_code = new_val; }

        uint8_t getEditingFlightPathErrorCode() { return editing_flight_path_error_code; }

        void setPathFollowingErrorCode(uint8_t new_val) { path_following_error_code = new_val; }

        uint8_t getPathFollowingErrorCode() { return path_following_error_code; }

        void setCurrentWaypointId(uint8_t new_val) { current_waypoint_id = new_val; }

        int getCurrentWaypointId() { return current_waypoint_id; }

        void setCurrentWaypointIndex(uint8_t new_val) { current_waypoint_index = new_val; }

        int getCurrentWaypointIndex() { return current_waypoint_index; }

        void setHomeBaseInitialized(uint8_t new_val) { home_base_initialized = new_val; }

        bool getHomeBaseInitialized() { return home_base_initialized; }

        /*
         * Getters and setters for Landing Mode Telemetry Data
         */
         void setIsLanded(bool new_val) { is_landed = new_val; }

         bool getIsLanded() { return is_landed; }

    private: 
        PathModeSelector();
        static PathModeSelector* singleton;

        PathMode* current_mode;
        PathModeEnum current_mode_enum;

        AltitudeAirspeedInput_t altitude_airspeed_input;
        CoordinatedTurnInput_t coordinated_turn_input;
        _PassbyControl passby_control_output;

        bool is_error;

        // Cruising Mode Telemetry data
        uint8_t editing_flight_path_error_code;
        uint8_t path_following_error_code;
        int current_waypoint_id;
        int current_waypoint_index;
        bool home_base_initialized;

        // Landing Mode Telemetry data
        bool is_landed;
};

#endif
