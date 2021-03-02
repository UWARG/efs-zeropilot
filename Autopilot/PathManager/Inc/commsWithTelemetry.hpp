/**
 * Communication with Telemetry Mode: sends data to, and receives data from Telemetry.
 * Author: Laye Mehta
 */  

#include "pathDatatypes.hpp"

#ifndef COMMS_WITH_TELEMETRY_HPP
#define COMMS_WITH_TELEMETRY_HPP
struct CommsTelemetryError_t{
int error_code; 
}; 

//data that is to be sent from Path Manager to Telemetry
struct PMtoTelemetryData_t{
    int a; //example, will be updated
}; 

//sets state of error in CommsTelemetryError_t struct
//need to think of parameters for this function
CommsTelemetryError_t CommsWithTelemetry_Result(); 

//send data to telemetry
PMtoTelemetryData_t* PMToTelemetry_SendData(void); 

//Do any processing required (update struct that contains telemetry data and process it)
//Store data inside of the Telemetry_PIGO_t struct that is a parameter of this child class
Telemetry_PIGO_t* TelemetryToPM_UpdateData(void); 

#endif