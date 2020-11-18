/**
 * Waypoint Manager Methods and Helpers
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updaetd: November 2020
 */

#ifndef WAYPOINT_MANAGER_H
#define WAYPOINT_MANAGER_H

#include <stdlib.h>

#define ORBIT_FOLLOWING 0
#define LINE_FOLLOWING 1

#define PI 3.14159265359
#define MAX_PATH_APPROACH_ANGLE PI/2

#define PATH_BUFFER_SIZE 100

//Waypoint Types
#define DEFAULT_WAYPOINT 0 // Used to navigate flight path
#define WAYPOINT_UNUSED 1
#define HOLD_WAYPOINT 2 // Circle

// Placeholder: 
typedef struct {
    /*
        Contains:
            long 
            lat
            alt
            heading
    */    
} _Gps_Data;

// Stores error codes for the waypoint manager
enum _WaypointStatus {WAYPOINT_SUCCESS = 0, WAYPOINT_NOT_FOUND, WAYPOINT_PARAMETERS_NOT_DEFIND, UNDEFINED_FALIURE};

// Used in the waypointBufferStatus array to signal which elements are free
enum _WaypointBufferStatus {FREE = 0, FULL};

//Used to specify the modification type when updating the waypointBuffer array
enum _WaypointBufferUpdateType {ADD_WAYPOINT = 0, UPDATE_WAYPOINT, INSERT_WAYPOINT, DELETE_WAYPOINT};


/*
* Structure stores information about the waypoints along our path to the destination and back. 
*/
typedef struct {
    struct _WaypointData * next;      // Next waypoint
    struct _WaypointData * previous;  // Previous waypoint
    long double latitude;             // Latitude of waypoint
    long double longitude;            // Longitude of waypoint
    int altitude;                     // Altitude of waypoint
    float turnRadius;                 // If turn is commanded, this is the radius of the turn
    char type;
    char waypointId;                  // Id of the waypoint
    char arrayIndex;                  // Position of the waypoint within the waypointBuffer (array)
} _WaypointData;

/*
* Structure contains the data that will be returned to the Path Manager state manager. 
* This data will be used by the PID and coordinated turn engine to determine the commands to be sent to the Attitude Manager.
*/
typedef struct {
    uint16_t desiredHeading;            // Desired heading to stay on path
    int desiredAltitude;                // Desired altitude at next waypoint
    long double distanceToNextWaypoint; // Distance to the next waypoint (helps with airspeed PID)
    _WaypointStatus errorCode;          // Contains error codes
    bool isDataNew;                     // Notifies PID modules if the data in this structure is new
    uint32_t timeOfData;                // The time that the data in this structure was collected
} _WaypointManager_Data;

class WaypointManager {
    public: 
        /*
        * Constructor for this class
        * 
        * @param _WaypointData * initialWaypoints -> These waypoints will be used to initialize the waypointBuffer array
        * @param int numWaypoints -> Number of waypoints being initialized in the waypointBuffer array
        * @param _WaypointData* currentLocation -> Home base. 
        */
        WaypointManager(_WaypointData * initialWaypoints, int numWaypoints, _WaypointData* currentLocation);
        
        /*
        * Updates the _WaypointManager_Data structure with new values
        * 
        * @param _Gps_Data currentPosition -> contains the current coordinates, altitude, and heading
        */
        void get_next_directions(_Gps_Data currentPosition); 

        /*
        * Adds, inserts, updates, or deletes a single waypoint in the waypointBuffer array
        * 
        * @param _WaypointData* waypoint -> In the instance that we need to update, insert, or append a new waypoint, this will be used 
        * @param int waypointId -> the ID of the waypoint that will be updated or deleted
        * @param _WaypointBufferUpdateType updateType -> the type of modification to the waypointBuffer array (look above)
        * @param int previousId -> stores the ID of the waypoint that will come before the inserted waypoint. Set to 0 by default, so does not need to be passed
        * @param int nextId -> stores the ID of the waypoint that will come after the inserted waypoint. Set to 0 by default, so does not need to be passed
        */
        void update_path_nodes(_WaypointData* waypoint, int waypointId, _WaypointBufferUpdateType updateType, int previousId = 0, int nextId = 0);

        /*
        * Clears the waypointBuffer array.
        */
        void clear_path_nodes();

        /*
        * Returns the waypointBuffer array if requested
        */
        _WaypointData* get_waypoint_buffer(); //Returns the waypointBuffer array

        /*
        * Updates an instance of the _WaypointManager_Data structure using the newly calculated values
        * 
        * @param _WaypointManager_Data &Data -> Memory address for a structure that holds the data for the state machine
        */
        _WaypointManager_Data GetResult(_WaypointManager_Data &Data);

    private:
        //Stores waypoints
        _WaypointData waypointBuffer[100]; //Stores all waypoints
        _WaypointBufferStatus waypointBufferStatus[100] = {FREE}; //Keeps status of elements in waypointBuffer array
        int numWaypoints;
        
        //Home base
        _WaypointData homeBase;
        
        //Data that will be transferred
        uint16_t desiredHeading;
        int desiredAltitude; 
        long double distanceToNextWaypoint;
        long double desiredLatitude;
        long double desiredLongitude;
        _WaypointStatus errorCode;
        bool dataIsNew = false;

        //Status variables
        bool goingHome = false;
        _WaypointStatus errorStatus;
        bool inHold = false; //When this is true, plane will just circle in the sky until another command is given
        bool orbiting = false; //When this is true, the plane is orbiting

        //Helper Methods
        void follow_waypoints(_WaypointData * currentWaypoint, float* position, float heading);                         // Determines which of the methods below to call :))                      
        void follow_line_segment(_WaypointData * currentWaypoint, float* position, float heading);                      // In the instance where the waypoint after the next is not defined, we continue on the path we are currently on
        void follow_last_line_segment(_WaypointData * currentWaypoint, float* position, float heading);                 // In the instance where the next waypoint is not defined, follow previously defined path
        void follow_orbit(float* center, float radius, char direction, float* position, float heading);                 // Makes the plane follow an orbit with defined radius and direction
        void follow_straight_path(float* waypointDirection, float* targetWaypoint, float* position, float heading);     // Makes a plane follow a straight path (straight line following)
        float maintain_altitude(_WaypointData* cPath);                                                                  // Makes plane maintain altitude
        
        /*
        * Takes GPS long and lat data and converts it into coordinates (better for calculating headings and stuff)
        * 
        * @param long double longitude -> GPS longitide
        * @param long double latitude -> GPS latitude
        * @param float* xyCoordinates -> Array that will store the x and y coordinates of the plane
        */
        void get_coordinates(long double longitude, long double latitude, float* xyCoordinates); 
        
        int get_waypoint_index_from_id(int waypointId);                                   // If provided a waypoint id, this method finds the element index in the waypointBuffer array
        _WaypointData* initialize_waypoint();                                             // Creates a blank waypoint
        _WaypointData* initialize_waypoint_and_next();                                    // Creates a blank waypoint with the next waypoint defined
        void append_waypoint(_WaypointData* newWaypoint);                                 // Adds a waypoint to the first free element in the waypointBuffer (array)
        void insert_new_waypoint(_WaypointData* newWaypoint, int previousId, int nextId); // Inserts new waypoint in between the specified waypoints (identified using the waypoint IDs)
        void delete_waypoint(int waypointId);                                             // Deletes the waypoint with the specified ID
        void update_waypoint(_WaypointData* updatedWaypoint, int waypointId);             // Updates the waypoint with the specified ID
};

#endif




