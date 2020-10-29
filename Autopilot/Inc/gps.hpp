/**
 * Gps sensor interface and part number selector.
 * Author(s): Anthony Berbari, Dhruv Rawat
 */

#ifndef GPS_HPP
#define	GPS_HPP

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define NEO_M8 0

typedef struct
{
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    float utcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
    float groundSpeed; // in m/s
    int altitude; // in m
    int16_t heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
    uint8_t numSatellites;    // 1 Byte

    uint8_t sensorStatus; // 0 = no fix, 1 = gps fix, 2 = differential gps fix (DGPS) (other codes are possible)
    bool dataIsNew; // true if data has been refreshed since the previous time GetResult was called, false otherwise.
	bool timeIsValid;

} GpsData_t;

class Gps
{
	public:
		/**
		* Begins the process of collecting the sensor's data.
		* This is a non blocking function that returns right away.
		*/
        virtual void BeginMeasuring(void) = 0;

		/**
		* Gets the information about the aircraft's position (See GpsData_t struct).
		* This is a non blocking function that returns right away, either with new data,
		* or with old data (in case of old data, the dataIsNew flag of the result struct will be cleared).
		* @param[out]		Data 		pointer to the result struct.
		*/
        virtual void GetResult(GpsData_t &Data) = 0;
};

class NEOM8 : public Gps 
{
	public:
		NEOM8(const NEOM8*) = delete;
		NEOM8* GetInstance();

		/**
         * Triggers interrupt for new GPS measurement - stores raw data in variables and returns right away
         * */
		void BeginMeasuring();

		 /**GetResult should:
         * 1. Reset dataIsNew flag
         * 2. Transfers raw data from variables to struct
         * 3. Updates utcTime and status values in struct as well
         * */
		void GetResult(GpsData_t &Data);

	private:
		//Constructor
		NEOM8();

		//Static instance
		static NEOM8* gps_Instance;

		//Variables
		bool isDataNew = false;
		long double measuredLatitude, measuredLongitude;  // 8 Bytes
		float measuredUtcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
		float measuredGroundSpeed; // in m/s
		int measuredAltitude; // in m
		int16_t measuredHeading; // in degrees. Should be between 0-360 at all times, but using integer just in case
		uint8_t measuredNumSatellites;    // 1 Byte

		//Methods
		bool is_check_sum_valid(char *);
		void get_gps_data();
		void calculate_check_sum(uint8_t *);
};

#ifdef UNIT_TESTING
#include "gpsMock.hpp"
#endif


#endif

