/**
*   
*   Airspeed sensor functions
*
*   This header is an outline of basic functions of the airspeed
*   sensor. Generally, this sensor is a Pitot tube which uses a 
*   pressure differential to determine airspeed. 
*
*   Airspeed calculation should be done and returned in meters 
*   per second
*
*    Author: Annie LePage, Anthony Berbari
**/



#ifndef AIRSPEED_HPP
#define AIRSPEED_HPP

#include <cstdint>

/*
    Currently there is only one airspeed sensor used, if this is 
    changed to a different sensor, part numbers and a selection
    option should be added
*/

struct airspeedData_t 
{
    double airspeed;        // in m/s

    int sensorStatus;       // report any errors, possible malfunctions 
    bool isDataNew;         // is the data fresh?
    float utcTime;          // 4 Bytes. Time in seconds since 00:00 
};

class airspeed {
    public:
        /**
         *  Triggers interrupt for new airspeed measurement - stores 
         *  raw data in variables and returns right away
         * */
        virtual void Begin_Measuring() = 0; 

        /**GetResult should:
         *  1. Transfer raw data from variables to struct
         *  2. Update utcTime and status values in struct as well
         *  
         *
         *  Potentially:
         *  ensure that data acquired makes sense, has been
         *  gathered recently within reason (past 10s?)
         * */
        virtual void GetResult(airspeedData_t &Data) = 0; 
};

/***********************************************************************************************************************
 * Derived classes
 **********************************************************************************************************************/

class MPXV7002DP : public airspeed {
    public:
        MPXV7002DP(const MPXV7002DP*) = delete;
        static MPXV7002DP* GetInstance();
        /**GetResult should:
         * Moves values from the variables to the airspeed data struct.
         * Sets the dataNew variable to false after that.
         */
        void GetResult(airspeedData_t &Data);

        /**
         *  Don't use this function, the GetResult function is non blocking.
         */
        void Begin_Measuring() {}

        // Calculates the offset of the values coming from the sensor.
        // It does this by taking 10 measurements, averaging them, and then subtracting
        // the baseline value from them. The baseline voltage for no 
        // differential pressure should be 2.5V.
        void CalcOffset();
        
    private:
        // Singleton style constructor.
        MPXV7002DP();
        // Single instance of the MPXV7002DP.
        static MPXV7002DP* s_Instance; 

        // Get the voltage from the diff pressure sensor, after applying the 
        // offset. This value should be in the range of 0.0 to 5.0 (as that 
        //is all the sensor can give).
        float GetVoltage();
        // Calculates the differential pressure from the voltage. This is done using the inverse of the 
        // equation provided in the differential pressure sensor's datasheet. That equation is how the 
        // sensor calculates the signal voltage from the pressure. So we use the inverse to calculate the
        // pressure from the signal voltage.
        float GetDiffPressure();
        // Plugs the differential pressure into an equation to find airspeed. This is based on Bernoulli's equation.
        float GetAirspeed();
        // Gets the adc value 10 times, averages it, then returns that average.
        uint16_t GetAverage();
        // Returns the current time. This is used to get the time of a reading used in the airspeed data struct.
       // uint32_t GetCurrentTime();

        int offset = 0;
        float airspeed = 0;
        float utcTime = 0;
        int sensorStatus = 0;
        bool dataNew = false;
        
};


#ifdef UNIT_TESTING
#include "airspeed_Mock.hpp"
#endif

#ifdef SIMULATION
// This derived class hooks into the Simulink simulation rather than hardware
class SimulatedAirspeed : public airspeed
{
    public :
        void Begin_Measuring();
        void GetResult(airspeedData_t &Data);
};
#endif

#endif
