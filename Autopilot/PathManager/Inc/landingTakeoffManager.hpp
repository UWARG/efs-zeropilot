#pragma once

#include "waypointManager.hpp"
#include "vectorClass.hpp"
#include "AutoSteer.hpp"
#include "SensorFusion.hpp"
#include "pathDatatypes.hpp"

enum _FlightStage{ROLL=0, CLIMB, CRUISING, TRANSITION, SLOPE, FLARE, DECRAB, TOUCHDOWN};



enum _FlightStage{ROLL=0, CLIMB, CRUISING, TRANSITION, SLOPE, FLARE, DECRAB, TOUCHDOWN};

/************************
    LANDING SECTION
************************/

constexpr static double DISTANCE_OF_LANDING {10}; //in meters
constexpr static double METERS_PER_DEG_LAT {111320.0};
constexpr static double ANGLE_OF_LANDING {5}; //in degrees
constexpr static int STALL_SPEED_NO_PACKAGE {2}; //in m/s
constexpr static int STALL_SPEED_WITH_PACKAGE {4}; //in m/s
constexpr static double PI {3.14159265358979323846};
constexpr static int FLARE_ALTITUDE {2}; //in meters
constexpr static double DECRAB_ALTITUDE {0.7}; //in meters
constexpr static double TOUCHDOWN_ALTITUDE {0.05}; //in meters
constexpr static double LATITUDE_LONGITUDE_CONVERSION_CONSTANT {40075000.0};

/************************
    TAKEOFF CONSTANTS
************************/

constexpr static double EXIT_TAKEOFF_ALTITUDE {50}; //in meters, difference in altitude
constexpr static double ROTATION_SPEED_NO_PACKAGE {4}; //in meters per second
constexpr static double ROTATION_SPEED_WITH_PACKAGE {6}; //in meters per second
constexpr static double CLIMB_SPEED_NO_PACKAGE {5}; // in meters per second
constexpr static double CLIMB_SPEED_WITH_PACKAGE {7}; // in meters per second
constexpr static double DISTANCE_OF_TAKEOFF {100}; // in meters, just used to approximate the horizontal location


struct _LandingPath
{
    _PathData stoppingPoint;
    _PathData aimingPoint;
    _PathData intersectionPoint;
};

struct _LandingTakeoffInput
{
    Telemetry_PIGO_t telemetryData;
    SFOutput_t sensorOutput;
};

struct _LandingTakeoffOutput
{
    double desiredHeading;
    double desiredTrack;
    double useHeading; //pay attention
    double desiredAltitude;
    double desiredAirspeed;
    _PassbyControl controlDetails;
};


class LandingManager{
    
    public:
        /*
            This function takes the horizontal position of the aircraft and determines the desired altitude
            Projects the position to the vertical plane relative to the aimingPoint, intersectionPoint, and stoppingPoint
            Takes the one of the horizontal coordinates, plug it into the equation of the slope, get an altitude
            
            @param SFOutput_t input - this variable will hold the sensor fusion data which is used to calculate current position
            @param _PathData aimingPoint - this variable will hold the location of the aiming point
            @param _PathData intersectionPoint - this variable will hold the location of the intersectionPoint
            @param _PathData stoppingPoint - this variable will hold the location of the stoppingPoint

            @return double - this function will return the desiredAltitude for the position of the relative points
        */
        static double changingAltitude(Telemetry_PIGO_t input, _PathData aimingPoint, _PathData intersectionPoint, _PathData stoppingPoint);

        /*
            This function takes in if there is a package on board and returns an approach speed the aircraft should follow
        
            @param bool ifPackage - this boolean holds if there is a package on board the aircraft

            @return double - this function will return the desired approach speed for the aircraft
        */
        static double approachSpeed(bool ifPackage);

        /* 
            This function takes the stall speed and multiplies it by a constant to return a slow flight speed
        
            @param bool ifPackage - this boolean holds if there is a package on board the aircraft

            @return double - this function will return the slow flight speed for the aircraft
        */
        static double slowFlightSpeed(bool ifPackage);

        /*
            This function takes the coordinates of the landing spots and direction of landing
            Creates stopping points, aiming point, and intersection point

            @param Telemetry_PIGO_t input - this variable will hold the stopping point information, which is passed in through telemetry
            @param double currentAltitude - this variable holds the current altitude of the aircraft, which is used for the altitude of the intersection point

            @return _LandingPath - this function will return a structure that contains the GPS locations of the stopping, aiming, and intersection points
        */
        static _LandingPath createSlopeWaypoints(Telemetry_PIGO_t input, double currentAltitude);

        /*
            This function translates waypoint output data into landing takeoff output data
            Takes in a waypoint data out structure, reformats it, and outputs a landingTakeoffOutput instance

            @param _WaypointManager_Data_Out outputData - this variable will hold the output data that is outputted by waypointManager functions

            @return _LandingTakeoffOutput - this function will output a landing and takeoff output struct
        */
        static _LandingTakeoffOutput translateWaypointCommands(_WaypointManager_Data_Out outputData);

        /*
            This function translates landing takeoff data to output data
            Takes in a landingTakeoffOutput instance, translates its information into the 2 types of coordinated turn inputs

            @param _LandingTakeoffOutput* outputData - this pointer points to the output of the landing and takeoff states
            @param SFOutput_t* sensorOutput - this pointer points to the sensor output of sensorFusion
            @param IMUOutput_t* imuOutput - this pointer points to the imu output of sensorFusion
            @param CoordinatedTurnInput_t - this pointer points to the coorinated turn input structure that needs to be changed
            @param AltitudeAirspeedInput_t - this pointer points to the altitude and airspeed input structure that needs to be changed
        */
        static void translateLTSFCommandsToCoordTurns(_LandingTakeoffOutput* outputData, SFOutput_t* sensorOutput, IMU_Data_t* imuOutput, CoordinatedTurnInput_t* turnInput, AltitudeAirspeedInput_t* altitudeAirspeedInput);
};

/************************
    TAKEOFF SECTION
************************/

constexpr static double EXIT_TAKEOFF_ALTITUDE {50}; //in meters, difference in altitude
constexpr static double ROTATION_SPEED_NO_PACKAGE {4}; //in meters per second
constexpr static double ROTATION_SPEED_WITH_PACKAGE {6}; //in meters per second
constexpr static double CLIMB_SPEED_NO_PACKAGE {5}; // in meters per second
constexpr static double CLIMB_SPEED_WITH_PACKAGE {7}; // in meters per second
constexpr static double DISTANCE_OF_TAKEOFF {100}; // in meters, just used to approximate the horizontal location

class TakeoffManager{

    public:
        /*
            This function returns the desired rotation speed limit, where the plane will transition from roll to climb
            
            @param bool ifPackage - this boolean holds if there is a package on board the aircraft

            @return double - this function will return the desired rotation speed for the aircraft
        */
        static double desiredRotationSpeed(bool ifPackage);

        /*
            This function returns the desired climb speed to be maintained throughout the climb
        
            @param bool ifPackage - this boolean holds if there is a package on board the aircraft

            @return double - this function will return the desired climb speed for the aircraft
        */
        static double desiredClimbSpeed(bool ifPackage);
        /*
            This function returns a waypoint that the plane should follow horizontally
            This waypoint does not have a vertical component, as max throttle is kept throughout the climb
        
            @param double currentLatitude - this variable holds the current latitude value of the aircraft
            @param double currentLongitude - this variable holds the current longitude of the aircraft
            @param double currentAltitude - this variable holds the current altitude of the aircraft
            @param double takeoffDirection - this variable holds the takeoffDirection of the aircraft

            @return _PathData - this structure holds the climb point, which the plane will follow until it exits at a certain altitude
        */
        static _PathData createTakeoffWaypoint(double currentLatitude, double currentLongitude, double currentAltitude, double takeoffDirection);
};

