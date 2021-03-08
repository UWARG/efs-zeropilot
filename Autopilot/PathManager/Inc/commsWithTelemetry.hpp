/**
 * Communication with Telemetry Mode: sends data to, and receives data from Telemetry.
 * Author: Laye Mehta and Avani Joshi
 */  

#include "pathDatatypes.hpp"
#include "CommWithPathManager.hpp"

#ifndef COMMS_WITH_TELEMETRY_HPP
#define COMMS_WITH_TELEMETRY_HPP


struct eulerAnglesZXY_t {
    float yaw, pitch, roll;
};

struct Telemetry_POGI_t{

    int errorCode;

    Telemetry_Waypoint_Data_t waypoint_data;

    int currentAltitude;
    int currentAirspeed;

    eulerAnglesZXY_t eulerAnglesOfPlane;
    eulerAnglesZXY_t eulerAnglesOfCamera;

    bool beginLanding;
};

//data that is to be sent from Path Manager to Telemetry
struct PMtoTelemetryData_t{
  //  int a; //example, will be updated
    float roll,pitch,yaw;	// commanded orientation (radians)
	float airspeed;			// commanded airspeed (m/s)
}PMtoTelemetryData_t; 

//sets state of error in CommsTelemetryError_t struct
//need to think of parameters for this function
int CommsWithTelemetry_Result(Telemetry_POGI_t comm_info); 

//send data to telemetry
PMtoTelemetryData_t* PMToTelemetry_SendData(); 

//Do any processing required (update struct that contains telemetry data and process it)
//Store data inside of the Telemetry_PIGO_t struct that is a parameter of this child class
Telemetry_PIGO_t* TelemetryToPM_UpdateData(); 

#endif