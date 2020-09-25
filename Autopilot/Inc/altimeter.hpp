/**
 * Altimeter Sensor Functions
 * Author: Lucy Gong
 */

#ifndef ALTIMETER_HPP
#define ALTIMETER_HPP

#include <ctime>

struct AltimeterData_t {

    float pressure, altitude, temp;

    bool isDataNew; 
    int status; //TBD but probably 0 = SUCCESS, -1 = FAIL, 1 = BUSY 
    uint32_t utcTime; //Last time GetResult was called
};

class Altimeter{
    public:
        /**
         * Initializes Altimeter
         * */
        virtual void Init() = 0; 

        /**
         * Triggers interrupt for new altimeter measurement - stores raw data in variables and returns right away
         * */
        virtual void Begin_Measuring() = 0; 

        /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
        virtual void GetResult(AltimeterData_t *Data) = 0; //
};

class MS5637 : public Altimeter {
    public:
        void Init();
        void Begin_Measuring();
        void GetResult(AltimeterData_t *Data);
    private:
        uint32_t readFromMS5637(uint32_t commandToWrite);
        void getRawPressureAndTemperature(int64_t *rawPressure, int64_t *rawTemperature);
        uint32_t getCurrentTime();
        float getTemperature();
        float getPressure();
        float getAltitude();
        
        static float altitudeMeasured, pressureMeasured, temperatureMeasured;
        uint32_t timeOfResult;
        static bool isI2CBusDefined;
        bool dataIsNew;


};

#endif
