/**
 * Gps sensor interface and part number selector.
 * Author: Anthony Berbari
 */

#ifndef GPS_HPP
#define	GPS_HPP

#include <stdint.h>

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

} GpsData_t;

class Gps
{
	public:

		/**
		* Initialises internal parameters.
		* Should be called exactly once before anything is attempted to be done with the module.
		*/
		virtual void Init(void) = 0;

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
        virtual void GetResult(GpsData_t *Data) = 0;
};

#endif
