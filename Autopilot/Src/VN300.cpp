/**
 * Implementations of the VN300 drivers header file
 */

#include "VN300.hpp"
#include "../lib/vn/Inc/vector.h"

//Implementation of the VN300 GPS driver class.

//Private constructor to initialize the only instance.
VN300_GPS::VN300_GPS(){
    
    //Register our callback function to run anytime a new packet is found
    pf.registerPossiblePacketFoundHandler(NULL, vn::protocol::uart::PacketFinder::ValidPacketFoundHandler(packetFoundCallback));

    // Initialize UART connection.
    
    //Write to any configuration registers.
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
    return this->gpsData;
}

void VN300_GPS::setGPSData(){
    //Generate a read register command
    numOfBytes = vn::protocol::uart::Packet::genReadGpsSolutionLla(vn::protocol::uart::ERRORDETECTIONMODE_CHECKSUM, buffer, sizeof(buffer));
    
    //Use UART connection to send request to VN

    //Wait for received UART data

    //Hand it over to our packet callback function
    pf.processReceivedData(buffer, numOfBytes);
}

void packetFoundCallback(void * data, vn::protocol::uart::Packet & packet, size_t packetStartIndex, vn::xplat::TimeStamp timestamp){
    
    // For brevity.
    using namespace vn::protocol::uart;

    // This function gets called after validation from the VN300 Library.
    // No checksum needed.

    //First, set up variables to hold onto the values we read from the register.
    double * time;
    uint16_t* week;
    uint8_t* gnssFix;
    uint8_t* numSats;

    // The next 3 variables use the the built-in vectornav vector library to hold onto values
    vn::math::vec3d* lla;
    vn::math::vec3f* nedVel;
    vn::math::vec3f* nedAcc;

    //Accuracy variables
    float* speedAcc;
    float* timeAcc;

    //First, we need to determine if this is a ASCII or binary packet.
    
    // ASCII packets are a little easier to process since we have access to the VN300 helpers
    if (packet.type() == Packet::TYPE_ASCII){
        
        //Ensure that this is a syncronous packet to parse
        if (packet.isResponse()){
            
            // We can use the built-in vectornav parser to extract data.
            packet.parseGpsSolutionLla(time, week, gnssFix, numSats, lla, nedVel, nedAcc, speedAcc, timeAcc);
            
            //We cannot make this function a class function due to the VN300 library. So, we 
            //call for the sensor instance and populate the struct.
            VN300_GPS* instance = VN300_GPS::GetInstance();

            //Put the data in our custom gps data struct.
            instance->gpsData.latitude = lla->x;
            instance->gpsData.longitude = lla->y;
            instance->gpsData.altitude = lla->z;

            instance->gpsData.northVel = nedVel->x;
            instance->gpsData.eastAcc = nedVel->y;
            instance->gpsData.downAcc = nedVel->z;

            instance->gpsData.northAcc = nedAcc->x;
            instance->gpsData.eastAcc = nedAcc->y;
            instance->gpsData.downAcc = nedAcc->z;

            instance->gpsData.time = *time;
            instance->gpsData.week = *week;
            instance->gpsData.gnssFix = *gnssFix;
            instance->gpsData.numSats = *numSats;
        }
    }
}