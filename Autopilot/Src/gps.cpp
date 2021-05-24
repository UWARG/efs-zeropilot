/*
 *  Author: Dhruv Rawat
 * 
 *  This code controls our GPS sensor and ensures we get samples at around 1 Hz.
 * 
 *  Future development: 
 *      - Configure GPS to increase baud rate from 9600 and reduce the amount of NMEA messages sent to just GGA and VTG
 *      - Make the GGA and VTG messages use GP instead of GN
 * 
 *  Parsing algorithms taken from the custom WARG PicPilot software. 
 */

#include <gps.hpp>
#include <cstring>
#include "stm32f7xx_hal.h"

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F\r\n"
#define PMTK_SET_NMEA_UPDATE_5HZ "$PMTK220,200*2C\r\n"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F\r\n"

// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_1HZ "$PMTK100,1000,0,0,0,0*1C\r\n"
#define PMTK_API_SET_FIX_CTL_5HZ "$PMTK100,200,0,0,0,0*2F\r\n"
// Can't fix position faster than 5 times a second!

#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C\r\n"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17\r\n"

//turn on VTG (velocity info) and GGA (positional info) only
#define PMTK_SET_NMEA_OUTPUT_GGAVTG "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

// one of the 2 DGPS modes
#define PMTK_ENABLE_SBAS "$PMTK313,1*2E\r\n"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E\r\n"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31\r\n"

// request for updates on antenna status
#define PGCMD_ANTENNA "$PGCMD,33,1*6C\r\n"
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D\r\n"

// https://nmeachecksum.eqth.net --> get checksums
#define PUBX_CONFIG_NMEA "$PUBX,41,1,07,03,9600,0*10\r\n" //Change hex numbers later
#define PUBX_SET_GGA "$PUBX,40,GGA,0,1,0,0,0,0*5B\r\n"
#define PUBX_SET_VTG "$PUBX,40,VTG,0,1,0,0,0,0*5F\r\n"
#define PUBX_SET_RMC_OFF "$PUBX,40,RMC,0,0,0,0,0,0*47\r\n"
#define PUBX_SET_GSA_OFF "$PUBX,40,GSA,0,0,0,0,0,0*4A\r\n"
#define PUBX_SET_GNS_OFF "$PUBX,40,GNS,0,0,0,0,0,0*41\r\n"
#define PUBX_SET_GLL_OFF "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n"

#define GPS_GGA_MESSAGE "GNGGA"
#define GPS_VTG_MESSAGE "GNVTG"

extern UART_HandleTypeDef huart4;

NEOM8* NEOM8::gpsInstance = nullptr;

NEOM8* NEOM8::GetInstance() {
	if (!gpsInstance) {
		gpsInstance = new NEOM8 {};
	}

	return gpsInstance;
}

NEOM8::NEOM8() : gpsData {},
				 configured {false},
				 dataAvailable {false} {

//	const uint8_t CFG_NMEA[16] = { 0x17, 0x20, 0b00011000, 0x40, 0x08, 0x01, 0x00, 0x00, 0x00, 0b01110110, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00 };
//	HAL_UART_Transmit_DMA(&huart4, (uint8_t*) CFG_NMEA, sizeof(CFG_NMEA));
//
//	HAL_UART_Transmit_DMA(&huart4, (uint8_t*) PUBX_CONFIG_NMEA, sizeof(PUBX_CONFIG_NMEA));
//
//	//	const uint8_t CFG_UART[21] = { 0x06, 0x00, 0x01, 0x00, 0b00000000, 0b00000000, 0x00, 0x00, 0b11000000, 0b00001000, 0x00, 0x01, 0b11000010, 0x00, 0x00, 0b00000010, 0x00, 0b00000010, 0x00, 0x00, 0x00 };
//	//	HAL_UART_Transmit_DMA(&huart4, CFG_UART, sizeof(CFG_UART));
//
//	HAL_UART_Transmit_DMA(&huart4, (uint8_t*) PMTK_SET_NMEA_UPDATE_10HZ, sizeof(PMTK_SET_NMEA_UPDATE_10HZ));
//
//	HAL_UART_Transmit_DMA(&huart4, (uint8_t*) PMTK_API_SET_FIX_CTL_5HZ, sizeof(PMTK_API_SET_FIX_CTL_5HZ));
//
//	HAL_UART_Transmit_DMA(&huart4, (uint8_t*) PMTK_ENABLE_WAAS, sizeof(PMTK_ENABLE_WAAS));

	configured = true;

	HAL_UART_Receive_DMA(&huart4, byte_collection_buffer, GPS_UART_BUFFER_SIZE);
}

/**
 * Method is called when our uart buffer (byte_collection_buffer) is full
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	NEOM8 * neoM8N = NEOM8::GetInstance();

	neoM8N->parse_gpsData();

	HAL_UART_Receive_DMA(&huart4, neoM8N->get_byte_collection_buffer(), GPS_UART_BUFFER_SIZE);
}

uint8_t* NEOM8::get_byte_collection_buffer() {
	return byte_collection_buffer;
}


bool NEOM8::is_check_sum_valid(uint8_t* string){
    uint16_t i = 0;
    uint8_t checksum = 0;

    while(string[i] != '*'){
        checksum ^= string[i];
        i++;
    }
    i++;

    return uint8_to_hex((checksum & 0xF0) >> 4) == string[i] &&  uint8_to_hex(checksum & 0x0F) == string[i+1];
}

uint8_t NEOM8::uint8_to_hex(unsigned int checkSumHalf) {
    uint8_t uint8_tOut = 0;

    if (checkSumHalf >= 0 && checkSumHalf <= 9){
        uint8_tOut = checkSumHalf + 0x30;
    }
    else if (checkSumHalf >= 0xA && checkSumHalf <= 0xF){
        uint8_tOut = checkSumHalf + 0x37;
    }
    return uint8_tOut;
}

uint8_t NEOM8::ascii_to_hex(uint8_t asciiSymbol) {
    uint8_t hexOut = 0;
    if (asciiSymbol == 0x2E)
        hexOut = 0x10;
    else if (asciiSymbol >= 0x30 && asciiSymbol <= 0x39){
        hexOut = asciiSymbol - 0x30;
    }
    else if (asciiSymbol >= 0x41 && asciiSymbol <= 0x46){
        hexOut = asciiSymbol - 0x37; //Letter "F"(ASCII 0x46) becomes 0xF
    }
    return hexOut;
}

void NEOM8::parse_gpsData() {
	static bool currently_parsing = false;
	static uint16_t buffer_index = 0;
	int b = 0;
	int c = 0;

	for (int i = 0; i < GPS_UART_BUFFER_SIZE; i++) {
		if (byte_collection_buffer[i] == '$') { //Beginning of Packet
			currently_parsing = true;
			buffer_index = 0;
		} else if (byte_collection_buffer[i] == '\r') { //End of Packet
			 if (strncmp((char*) GPS_GGA_MESSAGE, (char*) uart_buffer, 5) == 0){
				memcpy(gga_buffer, uart_buffer, GPS_UART_BUFFER_SIZE);
				gpsData.ggaDataIsNew = true;
				b += 10;
			 } else if (strncmp((char*) GPS_VTG_MESSAGE, (char*) uart_buffer, 5) == 0){
				memcpy(vtg_buffer, uart_buffer, GPS_UART_BUFFER_SIZE);
				gpsData.vtgDataIsNew = true;
				c += 20;
			 }
			 currently_parsing = false;

			 // Exit if we find both of the messages to not waste time on computation
			 if (gpsData.ggaDataIsNew && gpsData.vtgDataIsNew) {
				 break;
			 }
		} else if (currently_parsing){
			uart_buffer[buffer_index] = byte_collection_buffer[i];
			buffer_index = (buffer_index + 1) % GPS_UART_BUFFER_SIZE; //make sure we dont cause a segmentation fault here
		}
	}

	if (!gpsData.ggaDataIsNew && !gpsData.vtgDataIsNew){ //if no data has been copied over
		return;
	}

	if (gpsData.ggaDataIsNew){
		gpsData.ggaDataIsNew = false;
		if (is_check_sum_valid(gga_buffer)){
			dataAvailable = false;
			parse_gga(gga_buffer);
			dataAvailable = true;
			gpsData.dataIsNew = true;
			gpsData.timeIsNew = true;
			gpsData.ggaDataIsNew = true;
			gpsData.sensorStatus = 0;
		} else {
			gpsData.sensorStatus = 1;
		}
	}

	if (gpsData.vtgDataIsNew){
		gpsData.vtgDataIsNew = false;
		if (is_check_sum_valid(vtg_buffer)){
			dataAvailable = false;
			parse_vtg(vtg_buffer);
			dataAvailable = true;
			gpsData.dataIsNew = true;
			gpsData.vtgDataIsNew = true;
			gpsData.sensorStatus = 0;
		} else {
			gpsData.sensorStatus = 1;
		}
	}
}

void NEOM8::parse_vtg(uint8_t* data) {
	//static so that we dont allocate these variables every time
	static uint8_t rawHeading[6] = {0, 0, 0, 0, 0, 0};
	static uint8_t rawGroundSpeed[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	int comma = 0;
	int i = 0;
	int j = 0;

	while (data[j] != '*') {
		uint8_t numData = ascii_to_hex(data[j]);
		if (data[j] == ',') {
			comma++;
			i = 0;
		}

		if (comma == 1 && (i != 0)) {
			rawHeading[i] = numData;
		} else if (comma == 7 && (i != 0)) {
			rawGroundSpeed[i] = numData;
		}

		i++;
		j++;
	}

	i = 1;
	long int multiplier = 10;
	int decimalPoint = 0;

	gpsData.heading = 0;
	float tHeading = 0;
	for (i = 1; i < 6; i++) //this code first generates an 5 digit decimal number
	{
		if (rawHeading[i] == 0x10)//check for decimal point
		{
			decimalPoint = i;
		} else {
			tHeading += (float) (rawHeading[i]*100000 / multiplier);
			multiplier *= 10;
		}
	}
	decimalPoint = decimalPoint - 2;
	multiplier = 10000;
	while (decimalPoint > 0) //then divdes it according to the placement of the decimal
	{
		multiplier = multiplier / 10;
		decimalPoint--;
	}
	gpsData.heading = (int)(tHeading / multiplier);

	//	//calculate speed - tricky because of unknown 1-3 digits preceeding the decimal
	i = 1;
	multiplier = 10;
	decimalPoint = 0;
	gpsData.groundSpeed = 0;
	for (i = 1; i < 7; i++) //this code first generates an 6 digit decimal number
	{
		if (rawGroundSpeed[i] == 0x10)//check for decimal point
		{
			decimalPoint = i;
		} else {
			gpsData.groundSpeed += (float) (rawGroundSpeed[i]*1000000 / multiplier);
			multiplier = multiplier * 10;
		}
	}
	decimalPoint = decimalPoint - 2;
	multiplier = 100000;
	while (decimalPoint > 0) //then divdes it according to the placement of the decimal
	{
		multiplier = multiplier / 10;
		decimalPoint--;
	}
	gpsData.groundSpeed = gpsData.groundSpeed / multiplier;
}

void NEOM8::parse_gga(uint8_t* data) {
	int comma = 0; //comma counting so that we know what header we're parsing for
	int i = 0; //index for the current position of the field value
	int j = 0; //7th uint8_tacter is where data will start. index for the byte index in the counter

	//statically initialize placeholders for values. Static to avoid reinitializations every time
	static uint8_t rawTime[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static uint8_t rawLatitude[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static uint8_t rawLongitude[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static uint8_t rawSatellites[3] = {0, 0, 10};
	static uint8_t rawAltitude[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	static uint8_t latitudeNS = 0;
	static uint8_t longitudeEW = 0;
	static uint8_t positionFix = 0;

	while (data[j] != '*') {
		uint8_t numData = ascii_to_hex(data[j]);

		if (data[j] == ',') {
			comma++;
			i = 0;
		}

		if ((comma == 1) && (i != 0)) {
			rawTime[i] = numData;
		} else if ((comma == 2) && (i != 0)) {
			rawLatitude[i] = numData;

		} else if ((comma == 3) && (i != 0)) {
			latitudeNS = data[j];

		} else if ((comma == 4) && (i != 0)) {
			rawLongitude[i] = numData;

		} else if ((comma == 5) && (i != 0)) {
			longitudeEW = data[j];

		} else if ((comma == 6) && (i != 0)) {
			positionFix = numData;

		} else if ((comma == 7) && (i != 0)) {
			rawSatellites[i] = numData;
		} else  if ((comma == 9) && (i != 0)) {
			rawAltitude[i] = numData;
		}

		i++;
		j++;
	}

	//now we've got all the valid data placed in our buffers. Modify gps data struct to match

	//calculate time
	gpsData.utcTime = (float) rawTime[1] * 100000;
	gpsData.utcTime += (float) rawTime[2] * 10000;
	gpsData.utcTime += (float) rawTime[3] * 1000;
	gpsData.utcTime += (float) rawTime[4] * 100;
	gpsData.utcTime += (float) rawTime[5] * 10;
	gpsData.utcTime += (float) rawTime[6] * 1;
	//Decimal Point
	gpsData.utcTime += (float) rawTime[8] * 0.1;
	gpsData.utcTime += (float) rawTime[9] * 0.01;
	gpsData.utcTime += (float) rawTime[10] * 0.001;

	//calculate latitude
	gpsData.latitude = rawLatitude[3]*10.0;
	gpsData.latitude += rawLatitude[4]*1.0;
	gpsData.latitude += rawLatitude[6]*0.1;
	gpsData.latitude += rawLatitude[7]*0.01;
	gpsData.latitude += rawLatitude[8]*0.001;
	gpsData.latitude += rawLatitude[9]*0.0001;
	gpsData.latitude /= 60;  //Converts from dd.mmmmmm to decimal degrees. (60 minutes in a degree)
	//Then add the degrees (ranges from -90 to +90)
	gpsData.latitude += rawLatitude[1]*10.0;
	gpsData.latitude += rawLatitude[2]*1.0;

	if (latitudeNS == 'S'){
		gpsData.latitude *= -1;
	}

	//calculate longitude
	gpsData.longitude = rawLongitude[4]*10.0;
	gpsData.longitude += rawLongitude[5]*1.0;
	gpsData.longitude += rawLongitude[7]*0.1;
	gpsData.longitude += rawLongitude[8]*0.01;
	gpsData.longitude += rawLongitude[9]*0.001;
	gpsData.longitude += rawLongitude[10]*0.0001;
	gpsData.longitude /= 60;  //Converts from ddd.mmmmmm to decimal degrees. (60 minutes in a degree)
	//Then add the degrees (ranges from -180 to +180)
	gpsData.longitude += rawLongitude[1]*100.0;
	gpsData.longitude += rawLongitude[2]*10.0;
	gpsData.longitude += rawLongitude[3]*1.0;

	if (longitudeEW == 'W'){
		gpsData.longitude *= -1;
	}

	//calculate satellites
	if (rawSatellites[2] == 10) gpsData.numSatellites = rawSatellites[1];
	else gpsData.numSatellites = rawSatellites[1]*10 + rawSatellites[2];

	//calculate altitude - tricky because of unknown 1-3 digits preceeding the decimal
	i = 1;
	long int multiplier = 10;
	int decimalPoint = 0;
	gpsData.altitude = 0;
	float tAltitude = 0;
	for (i = 1; i < 8; i++) //this code first generates an 6 digit decimal number
	{
		if (rawAltitude[i] == 0x10) //check for decimal point
		{
			decimalPoint = i;
		} else {
			tAltitude += (float) (rawAltitude[i]*1000000 / multiplier);
			multiplier *= 10;
		}
	}
	decimalPoint = decimalPoint - 2;
	multiplier = 100000;
	while (decimalPoint > 0) //then divides it according to the placement of the decimal
	{
		multiplier = multiplier / 10;
		decimalPoint--;
	}
	gpsData.altitude = (int)(tAltitude / multiplier);
}

void NEOM8::GetResult(GpsData_t * Data) {
	if (dataAvailable) {
		Data->dataIsNew = gpsData.dataIsNew;
		Data->ggaDataIsNew = gpsData.ggaDataIsNew;
		Data->vtgDataIsNew = gpsData.vtgDataIsNew;
		Data->latitude = gpsData.latitude;
		Data->longitude = gpsData.longitude;
		Data->utcTime = gpsData.utcTime;
		Data->groundSpeed = gpsData.groundSpeed;
		Data->altitude = gpsData.altitude;
		Data->heading = gpsData.heading;
		Data->numSatellites = gpsData.numSatellites;
		Data->fixStatus = gpsData.fixStatus;
		Data->sensorStatus = gpsData.sensorStatus;
		Data->timeIsNew = gpsData.timeIsNew;

		gpsData.vtgDataIsNew = false;
		gpsData.ggaDataIsNew = false;
		gpsData.dataIsNew = false;
		gpsData.timeIsNew = false;
		dataAvailable = false;
	}
}





