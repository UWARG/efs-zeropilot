/**
 * Implementations of the VN300 drivers header file
 */

#include "VN300.hpp"

//Implementation of the VN300 GPS driver class.

//Private constructor to initialize the only instance.
VN300_GPS::VN300_GPS(){

}
//Function called to get the single instance of the class
VN300_GPS* VN300_GPS::GetInstance(){

    //if the current instance is null, create one
    if (instance == nullptr){
        instance = new VN300_GPS();
    }

    //Then return the instance (either new or pre-existing)
    return instance;
}

VN300_GPSData VN300_GPS::getGPSData(){
    return this->data;
}


void VN300_GPS::packetFoundCallback(void * data, vn::protocol::uart::Packet & packet, size_t packetStartIndex){
    
}