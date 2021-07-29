/**
 * Gps sensor interface and part number selector.
 * Author(s): Anthony Berbari, Dhruv Rawat
 */

#ifndef GPS_HPP
#define	GPS_HPP

#include <stdint.h>

/** What UART interface the device is connected to */
#define GPS_UART_INTERFACE 1

/** Baudrate the gps communicates over by default */
#define GPS_UART_BAUDRATE 9600

/** How large to make the internal buffer for parsing messages. Should be the size
 of the largest message we'll receive from the gps*/
#define GPS_UART_BUFFER_SIZE 800

#define NEO_M8 0

struct GpsData_t{
    long double latitude;  // 8 Bytes
    long double longitude; // 8 Bytes
    float utcTime;     // 4 Bytes. Time in seconds since 00:00 (midnight)
    float groundSpeed; // in m/s
    int altitude; // in m
    int16_t heading; // in degrees. Should be between 0-360 at all times, but using integer just in case
    uint8_t numSatellites;    // 1 Byte
	uint8_t fixStatus; //0 = No GPS, 1 = GPS fix, 2 = DGSP Fix, 3 = Estimated/Dead Recoking Fix

    uint8_t sensorStatus; // 0 = good, 1 = error
    bool dataIsNew; // true if data has been refreshed since the previous time GetResult was called, false otherwise.
	bool timeIsNew;

	//Added these so autopilot knows which data is new
	bool ggaDataIsNew; //Position, altitude, time, and number of satellites
	bool vtgDataIsNew; //Groundspeed and Heading

};

class Gps
{
	public:
		/**
		* Begins the process of collecting the sensor's data.
		* This is a non blocking function that returns right away.
		*/
//        virtual void BeginMeasuring(void) = 0;

		/**
		* Gets the information about the aircraft's position (See GpsData_t struct).
		* This is a non blocking function that returns right away, either with new data,
		* or with old data (in case of old data, the dataIsNew flag of the result struct will be cleared).
		* @param[out]		Data 		pointer to the result struct.
		*/
        virtual void GetResult(GpsData_t &Data) = 0;
};

/*
 * GPS spews data without us needing to ask for it
 */
class NEOM8 : public Gps
{
	public:
		NEOM8(const NEOM8*) = delete;
		static NEOM8* GetInstance();

		 /**GetResult should:
		 * 1. Reset dataIsNew flag
		 * 2. Transfers raw data from variables to struct
		 * 3. Updates utcTime and status values in struct as well
		 * */
		void GetResult(GpsData_t &Data);

		/**
		 * Returns the buffer used to receive GPS UART signals
		 */
		uint8_t* get_byte_collection_buffer();

		void set_uart_data_is_new(bool val);

	private:
		//Constructor
		NEOM8();

		//Static instance
		static NEOM8* gpsInstance;

		// Buffers
		uint8_t gga_buffer[GPS_UART_BUFFER_SIZE]; //buffer for pasing gga (positional packets)
		uint8_t vtg_buffer[GPS_UART_BUFFER_SIZE]; //buffer for parsing vtg packets (velocity packets)
		uint8_t uart_buffer[GPS_UART_BUFFER_SIZE]; //buffer for parsing vtg packets (velocity packets)
		uint8_t byte_collection_buffer[GPS_UART_BUFFER_SIZE];
		bool uartDataIsNew;

		bool dataAvailable;
		bool configured; //if the gps module has been initialized and configured
		GpsData_t gpsData;

		/**
		 * Given an NMEA string starting after the $, verifies the integrity of the string
		 * using the checksum
		 * @param string
		 * @return True if string is a valid gps string, false otherwise
		 */
		bool is_check_sum_valid(uint8_t* string);

		/**
		 * Methods used for simple conversions when parsing NMEA messages
		 */
		uint8_t uint8_to_hex(unsigned int checkSumHalf);
		uint8_t ascii_to_hex(uint8_t asciiSymbol);

		/**
		 * Parses the VTG NMEA message and populates the GpsData_t sruct
		 */
		void parse_vtg(uint8_t* data);

		/**
		 * Parses the GGA NMEA message and populates the GpsData_t sruct
		 */
		void parse_gga(uint8_t* data);

		/**
		 * Goes through byte_collection_buffer, finds the messages we care about, and calls the appropriate parsing functions
		 */
		void parse_gpsData();

};

#ifdef UNIT_TESTING
#include "gpsMock.hpp"

class TestGps : public Gps {
    public:
        static TestGps* GetInstance();

        void Begin_Measuring();
        void GetResult(GpsData_t &Data);
};

#endif

#ifdef SIMULATION
// This derived class hooks into the Simulink simulation rather than hardware
class SimulatedGps : public Gps
{
    public :
        void Begin_Measuring();
        void GetResult(GpsData_t *Data);
};
#endif

#endif

