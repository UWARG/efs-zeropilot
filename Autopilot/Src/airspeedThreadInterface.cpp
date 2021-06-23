#include "airspeedThreadInterface.h"
#include "airspeed.hpp"

// TODO: The implementations of the functions and integration into threads will be done once the airspeed driver is fully implemented

void airspeedThreadInterfaceInit(void) {
    static airspeed& asi = MPXV7002DP::GetInstance();
}

void airspeedThreadInterfaceExecute() {

} 

