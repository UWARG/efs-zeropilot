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
*    Author: Annie LePage
**/


#ifndef AIRSPEED_HPP
#define AIRSPEED_HPP

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

class airspeed 
{
    public:
        /**
         *  Initializes ADC in order to recieve sensor readings 
         *  (pitot tube reading)
         * */
        virtual void Init() = 0; 

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
        virtual void GetResult(airspeedData_t *Data) = 0; 
};

class dummyairspeed: public airspeed{
    public:
        /**
         *  Initializes ADC in order to recieve sensor readings 
         *  (pitot tube reading)
         * */
        virtual void Init(); 

        /**
         *  Triggers interrupt for new airspeed measurement - stores 
         *  raw data in variables and returns right away
         * */
        virtual void Begin_Measuring(); 

        /**GetResult should:
         *  1. Transfer raw data from variables to struct
         *  2. Update utcTime and status values in struct as well
         *  
         *
         *  Potentially:
         *  ensure that data acquired makes sense, has been
         *  gathered recently within reason (past 10s?)
         * */
        virtual void GetResult(airspeedData_t *Data);

};

#endif
