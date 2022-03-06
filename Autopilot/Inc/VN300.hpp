/**
 * VN300 Class to support the IMU and GPS drivers
 */

#ifndef VN300_HPP
#define VN300_HPP

#include <cstdlib>

#include "../lib/vn/Inc/packet.h"

template <typename Sensor>
class VN300 {

    public:
        Sensor getSensorInstance(){
            
            // If instance is null, get it and set it
            if (s_instance == nullptr){
                s_instance = Sensor::getInstance();
            }

            //return
            return s_instance;
        }

    private:

        //A pointer to the existing class instance - nullptr if it doesnt exist
        Sensor* s_instance;

};


struct VN300_GPSData{

    //Position defining variables
    double latitude; //in deg
    double longitude; //in deg
    double altitude; //in meters

    //Velocity defining variables - they are defined as north, east and downwards positive
    //All in m/s
    float northVel;
    float eastVel;
    float downVel;

    //Position accuracy estimates - indicate how accurate the position readings are
    //All in meters
    float northAcc;
    float eastAcc;
    float downAcc;

    //Other useful accuracy variables
    float speedAcc; //m/s
    float timeAcc; //sec

    //Other useful variables:

    //Time of the GPS weeks in seconds
    double time;

    //The GPS week (1 GPS calendar is 1023 weeks)
    uint16_t week;
    
    //GNSS Fix type - consult user manual for more info
    uint8_t gnssFix;

    //Number of GNSS satellites being used
    uint8_t numSats;
};


class VN300_GPS {

    public:

        //added to make sure people dont accidentally initialize
        VN300_GPS(const VN300_GPS*) = delete;

        //Pulls new gps data from the VN300
        void setGPSData();

        //Returns GPS data struct 
        VN300_GPSData getGPSData();

        /*
        * Called to either get the existing instance of the class, or create a new one if 
        * one currently does not exist
        */
        static VN300_GPS* GetInstance();

    private:

        //Private constructor to prevent multiple instances
        VN300_GPS();

        // Callback that runs everytime a datapacket is found
        void packetFoundCallback(void * data, vn::protocol::uart::Packet & packet, size_t packetStartIndex);

        int registerID;

        //A pointer to the existing class instance - nullptr if it doesnt exist
        static VN300_GPS* instance;

        //Instance of struct to hold the gps data
        VN300_GPSData data;

};
#endif