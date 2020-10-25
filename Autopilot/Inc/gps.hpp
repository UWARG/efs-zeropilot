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
		//Data from PVT
		unsigned char cls;
		unsigned char id;
		unsigned short len;
		unsigned long iTOW;          // GPS time of week of the navigation epoch (ms)
		
		unsigned short year;         // Year (UTC) 
		unsigned char month;         // Month, range 1..12 (UTC)
		unsigned char day;           // Day of month, range 1..31 (UTC)
		unsigned char hour;          // Hour of day, range 0..23 (UTC)
		unsigned char minute;        // Minute of hour, range 0..59 (UTC)
		unsigned char second;        // Seconds of minute, range 0..60 (UTC)
		char valid;                  // Validity Flags (see graphic below)
		unsigned long tAcc;          // Time accuracy estimate (UTC) (ns)
		long nano;                   // Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
		unsigned char fixType;       // GNSSfix Type, range 0..5
		char flags;                  // Fix Status Flags
		unsigned char reserved1;     // reserved
		unsigned char numSV;         // Number of satellites used in Nav Solution
		
		long lon;                    // Longitude (deg)
		long lat;                    // Latitude (deg)
		long height;                 // Height above Ellipsoid (mm)
		long hMSL;                   // Height above mean sea level (mm) 
		unsigned long hAcc;          // Horizontal Accuracy Estimate (mm)
		unsigned long vAcc;          // Vertical Accuracy Estimate (mm)
		
		long velN;                   // NED north velocity (mm/s)
		long velE;                   // NED east velocity (mm/s)
		long velD;                   // NED down velocity (mm/s)
		long gSpeed;                 // Ground Speed (2-D) (mm/s)
		long heading;                // Heading of motion 2-D (deg)
		unsigned long sAcc;          // Speed Accuracy Estimate
		unsigned long headingAcc;    // Heading Accuracy Estimate
		unsigned short pDOP;         // Position dilution of precision
		unsigned short additionalFlags;             // Reserved
		unsigned long reserved3;     // Reserved


		bool isDataNew = false;
		long double measuredLatitude, measuredLongitude;  // 8 Bytes
		float measuredUtcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
		float measuredGroundSpeed; // in m/s
		int measuredAltitude; // in m
		int16_t measuredHeading; // in degrees. Should be between 0-360 at all times, but using integer just in case
		uint8_t measuredNumSatellites;    // 1 Byte

		//Methods
		void get_gps_data();
		void get_gps_data_error();
		void calculate_check_sum(uint8_t *);
};

#ifdef UNIT_TESTING
#include "gpsMock.hpp"
#endif


#endif

