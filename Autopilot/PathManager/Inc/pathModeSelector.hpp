#ifndef PATH_MODE_SELECTOR_HPP
#define PATH_MODE_SELECTOR_HPP

#include "pathMode.hpp"
#include "TelemPathInterface.hpp"
#include "SensorFusion.hpp"
#include "AutoSteer.hpp"

enum PathModeEnum {MODE_TAKEOFF = 0, MODE_CRUISING, MODE_LANDING, MODE_TAXIING};

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
         *
         * @return none
         */
        void execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in);

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
        void setAltitdeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input);

        /**
         * get the altitude_airspeed_input parameter, which will be used by coordinateTurnElevation 
         * 
         * @return altitude_airspeed_input parameter
         */
        AltitudeAirspeedInput_t getAltitudeAirspeedInput() { return altitude_airspeed_input; }

        // Getters and setters for output data insert later

    private: 
        PathModeSelector();
        static PathModeSelector* singleton;

        PathMode* current_mode;
        PathModeEnum current_mode_enum;

        // Output data insert later
        AltitudeAirspeedInput_t altitude_airspeed_input;
};

#endif