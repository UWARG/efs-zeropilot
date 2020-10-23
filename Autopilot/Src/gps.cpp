/**
 * Gps sensor
 * Author(s): Dhruv Rawat
 */

#include "gps.hpp"
#include <stddef.h>

/*** REGISTER DEFINITION BEGINS ***/

/*** REGISTER DEFINITION ENDS ***/

NEOM8* NEOM8::gps_Instance = NULL;

/*** MAIN CODE BEGINS ***/

NEOM8* NEOM8::GetInstance() {
    if(!gps_Instance) {
        gps_Instance = new NEOM8;
    }

    return gps_Instance;
}

NEOM8::NEOM8() {

}

void NEOM8::get_gps_data() {
    
}

void NEOM8::BeginMeasuring() {
    get_gps_data();
}

void NEOM8::GetResult(GpsData_t &Data) {
    Data.dataIsNew = isDataNew;
    isDataNew = false;
    Data.latitude = measuredLatitude;
    Data.longitude = measuredLongitude;
    Data.utcTime = measuredUtcTime;
    Data.groundSpeed = measuredGroundSpeed;
    Data.altitude = measuredAltitude;
    Data.heading = measuredHeading;
    Data.numSatellites = measuredNumSatellites;
}

/*** MAIN CODE ENDS ***/
