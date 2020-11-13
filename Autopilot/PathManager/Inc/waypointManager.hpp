/**
 * Waypoint Manager Functions and Helpers
 * Author: Dhruv Rawat
 * Created: November 2020
 * Last Updaetd: November 2020
 */

#ifndef WAYPOINT_MANAGER_H
#define WAYPOINT_MANAGER_H

#include <stdlib.h>

/*
    Structure stores information about the waypoints along our path to the destination and back. 
*/

//Temp placeholder: 
typedef struct {
    /*
        Contains:
            long 
            lat
            alt
            heading
    */    
} _Gps_Data;


enum _WaypointStatus {WAYPOINT_SUCCESS = 0, WAYPOINT_NOT_FOUND, WAYPOINT_PARAMETERS_NOT_DEFIND};
enum _WaypointBufferStatus {FULL = 0, FREE};
enum _WaypointBufferUpdateType {ADD_WAYPOINT = 0, UPDATE_WAYPOINT, INSERT_WAYPOINT, DELETE_WAYPOINT};

typedef struct {
    struct _WaypointData * next; //Next waypoint
    struct _WaypointData * previous; //Previous waypoint
    long double latitude; //Latitude of waypoint
    long double longitude; //Longitude of waypoint
    int altitude; //Altitude of waypoint
    float turnRadius; //If turn is commanded, this is the radius of the turn
    char type; //Type of waypoint: T -> turn; O -> Orbit/Circle; P -> follow path (tbh this is tbd)
    char waypointId; //Id of the waypoint
    char arrayIndex; //Position of the waypoint within the waypointBuffer (array)
} _WaypointData;

/*
    Structure contains the data that will be returned to the Path Manager state manager. 
    This data will be used by the PID and coordinated turn engine to determine the commands to be sent to the Attitude Manager.
*/
typedef struct {
    uint16_t desiredHeading;
    int desiredAltitude; 
    long double distanceToNextWaypoint;
    long double desiredLatitude;
    long double desiredLongitude;
    _WaypointStatus errorCode;
    bool isDataNew;
    uint32_t timeOfData;
} _WaypointManager_Data;

class WaypointManager {
    public: 
        WaypointManager(_WaypointData * initialWaypoints, int num_waypoints, _WaypointData currentLocation);
        
        void get_next_directions(_Gps_Data currentPosition); //Updates the _WaypointManager_Data structure with new values
        void update_path_nodes(_WaypointData waypoint, int index, _WaypointBufferUpdateType updateType); //adds, inserts, updates, or deletes a single waypoint in the waypointBuffer array
        void clear_path_nodes(); //Empties the waypointBuffer array
        _WaypointData* get_waypoint_buffer(); //Returns the waypointBuffer array

        _WaypointManager_Data GetResult(_WaypointManager_Data &Data);
    private:
        //Stores waypoints
        _WaypointData waypointBuffer[100]; //Stores all waypoints
        _WaypointBufferStatus waypointBufferStatus[100]; //Keeps status of elements in waypointBuffer array
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
        bool inHold = false; //When this is true, plane will just circle in the sky

        //Helper Methods
        //Some stuff here ig
};



#endif




