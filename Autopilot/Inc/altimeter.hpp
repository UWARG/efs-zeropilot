/**
 * Altimeter Sensor Functions
 * Author: Lucy Gong
 */

#ifndef ALTIMETER_HPP
#define ALTIMETER_HPP

#include <ctime>
#include <cstdint>

struct AltimeterData_t {

    float pressure, altitude, temp;

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    uint32_t utcTime; //Last time GetResult was called
};

class Altimeter{
    public:
        /**
         * Triggers interrupt for new altimeter measurement - stores raw data in variables and returns right away
         * */
        virtual void Begin_Measuring() = 0; 

        /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        virtual void GetResult(AltimeterData_t& Data) = 0; //
};

class MS5637 : public Altimeter {
    public:
        MS5637(const MS5637*) = delete; //Apparently if you try to copy a singleton this will give you errors?
        static MS5637* GetInstance();
        void Begin_Measuring();
        void GetResult(AltimeterData_t& Data);
    private:
        MS5637(); //Constructor can never be called muwhahaha
        static MS5637* s_Instance;
        uint32_t readFromMS5637(uint32_t commandToWrite);
        void getRawPressureAndTemperature(float *displayPressure, float *displayTemperature, float *displayAltitude);
        uint32_t getCurrentTime();
        uint32_t timeOfResult;
        bool dataIsNew = false;
        float altitudeMeasured = 0, pressureMeasured = 0, temperatureMeasured = 0;


};

#ifdef UNIT_TESTING
#include "airspeed_Mock.hpp"

class TestAltimeter : public Altimeter {
    public:
        static TestAltimeter* GetInstance();

        void Begin_Measuring();
        void GetResult(AltimeterData_t& Data);
};
#endif

#ifdef SIMULATION
// This derived class hooks into the Simulink simulation rather than hardware
class SimulatedAltimeter : public Altimeter
{
    public :
        void Begin_Measuring();
        void GetResult(AltimeterData_t& Data);
};
#endif

#endif
