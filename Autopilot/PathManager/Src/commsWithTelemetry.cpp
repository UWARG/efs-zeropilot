/**
 * Communication with Telemetry Mode: sends data to, and receives data from Telemetry.
 * Author: Laye Mehta and Avani Joshi
 */  

 #include "commsWithTelemetry.hpp"

 int CommsWithTelemetry_Result(Telemetry_POGI_t comm_info){
     return comm_info.errorCode;
 }

 PMtoTelemetryData_t* PMToTelemetry_SendData(){
     
 } 

 Telemetry_PIGO_t* TelemetryToPM_UpdateData(){

 }