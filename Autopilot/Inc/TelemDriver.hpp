#ifndef TELEM_DRIVER_HPP
#define TELEM_DRIVER_HPP


#include <ctime>
#include <cstdint>

struct TelemToPMData {
    struct TelemToPMData { //Coming to telem from ground base.
        float gpsLattitude, gpsLongitude; //Commanded gps position
        float heading, distance //Commanded heading/distance
        float roll, pitch, yaw;	//Commanded orientation (radians)
        bool beginLand; //Command to start landing process
        bool beginTakeoff; //Starting command
        bool disconnectAutopilot; //Command to switch off autopilot
    };
};

class TelemManager { //Pulling data from ground
public:
    virtual void GetResult(TelemToPMData* Data) = 0;
};

class XBEE : public TelemManager {
public:
    XBEE(const XBEE*) = delete;
    static XBEE* GetInstance();
    void ConnectXbees();
    void GetResult(TelemToPMData* Data);
private:
    XBEE();
    static XBEE* s_Instance;
    uint32_t readFromXBEE(uint32_t commandsData);
    void getRawData(TelemToPMData* commandsData);
    //That's a lot of data to be recieved from ground, not sure if they need seperate implementation, so keeping this here: float* displaygpsLat, float* displaygpsLong, float* displayHeading, float* displayDistance, float* displayRoll, float* displayPitch, float* displayYaw, bool* LandCommand, bool* liftoffCommand, bool* disconnectAutopilotCommand
    uint32_t getCurrentTime();
    uint32_t timeOfResult;
    bool dataIsNew = false;
    float gpsLattitudeRead, gpsLongitudeRead; //Commanded gps position
    float headingRead, distanceRead //Commanded heading/distance
    float rollRead, pitchRead, yawRead;	//Commanded orientation (radians)
    bool beginLandRead; //Command to start landing process
    bool beginTakeoffRead; //Starting command
    bool disconnectAutopilotRead; //Command to switch off autopilot

};

#endif