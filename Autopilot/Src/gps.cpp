/**
 * Gps sensor
 * Author(s): Dhruv Rawat
 */

/*
    This video series is amazing:
    - https://www.youtube.com/watch?v=TwhCX0c8Xe0&t=2089s
    - https://www.youtube.com/watch?v=ylxwOg2pXrc
*/

#include "gps.hpp"
#include "UART.hpp"
#include <stddef.h>

#define CMS_TO_MPS 0.01f; //Converts from cm/s to m/s

//Total size in bytes: 64 bytes (This is the smallest arrangement I could find. Some were 92 bytes)

//NOTE that if these registers do not work, there is this on that will give all of our info from one register. Downside is it's 92 bytes
#define UXB_NAV_PVT 0x07; //ID of register. Returns all data we need, but is 92 bytes in size.
#define PVT_LENGTH 92;
// #define UXB_NAV_VENED 0x12; //ID of register. Returns 36 bytes containing heading and groundspeed. I DONT HAVE BINARIES FOR THIS ONE. WILL NEED TO USE WINDOWS APP
// #define UBX_NAV_POSLLH 0x02; //ID of register. Returns 28 bytes containing position and altitude.
// #define VELNED_LENGTH 36;
// #define POSLLH_LENGTH 28;

#define UBX_HEADER_1 0xB5;
#define UBX_HEADER_2 0x62;
#define UBX_NAV_CLASS 0x01;

NEOM8* NEOM8::gps_Instance = NULL;
UARTPort *gpsUART;

/*** MAIN CODE BEGINS ***/

NEOM8* NEOM8::GetInstance() {
    if(!gps_Instance) {
        gps_Instance = new NEOM8;
    }
    return gps_Instance;
}

NEOM8::NEOM8() {
    UARTSettings gpsUARTConfig;
    gpsUARTConfig.parity = UART_NO_PARITY;

    gpsUART = new UARTPort(UART_PORT1, gpsUARTConfig);
    gpsUART->reset();
    gpsUART->setup();

    //I CANNOT FIND THE BINARIES FOR THE UBX_NAV_VELNED 
    uint8_t configureUBX[] = { 
        //Disable NMEA
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
        0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47, // GxVTG off

        // Disable UBX
        0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
        // 0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off

        // Enable UBX
        0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
        // 0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on

        //RATE
        0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12, //Frequency of GPS set to 10 Hz
    };

    gpsUART->transmit(configureUBX, sizeof(configureUBX)); //Sends config binaries 
    HAL_Delay(10); //Delay for 10 ms to allow for GPS to boot up
}

void NEOM8::get_gps_data() {
    uint8_t gps_raw_data[184]; //Stores the data from the register. I know it is a lot :((. Once I get the binaries for VELNED, I can reduce this to 64 bytes
    gps_raw_data[0] = 0xB5;
    gps_raw_data[1] = 0x62;
    gps_raw_data[2] = 0x92;
    gps_raw_data[3] = 0x07;

    uint8_t checkSum[2] = {0x00, 0x00};

    /*
        Still unclear if this is the correct way to get the data becasue from what I saw from tutorials (all of which used arduino Serial output), the GPS
        spits data to the microcontroller and you have to wait until the HEADER bytes (0xB5 and 0x62) appear in succession before starting a measurement. 

        I don't know if using this UART interface will only get data when it is called or now. 
    */

    size_t bytesRead = 0;
    gpsUART->read_bytes(gps_raw_data, sizeof(gps_raw_data), bytesRead); //Gets the data over the UART interface

    bool found = false;
    int startingIndex = 0;
    for(int i = 0 ; i < 92 && !found; i++) {
        if(gps_raw_data[i] == 0xB5 && gps_raw_data[i+1] == 0x62) {
            startingIndex = i;
            found = true;
        }
    }

    //Page 332 of datasheet (it is byte offset + 6 for the element. This is because the gps returns 6 bytes for header (2), class, ID, and length)
    cls = gps_raw_data[startingIndex + 2];
    id = gps_raw_data[startingIndex + 3];
    len = gps_raw_data[startingIndex + 4] << 8 | gps_raw_data[startingIndex + 5];
    iTOW = gps_raw_data[startingIndex + 6] << 24 | gps_raw_data[startingIndex + 7] << 16 | gps_raw_data[startingIndex + 8] << 8 | gps_raw_data[startingIndex + 9]; 
    
    if(cls == 0x01 && id == 0x07) {
        //Time
        year = gps_raw_data[startingIndex + 10] << 8 | gps_raw_data[startingIndex + 11];
        month = gps_raw_data[startingIndex + 12];
        day = gps_raw_data[startingIndex + 13];
        hour = gps_raw_data[startingIndex + 14];
        minute = gps_raw_data[startingIndex + 15];
        second = gps_raw_data[startingIndex + 16];
        //Validity
        valid = gps_raw_data[startingIndex + 17];
        //Number of satellites
        numSV = gps_raw_data[startingIndex + 29];
        //Position
        lon = gps_raw_data[startingIndex + 30] << 24 | gps_raw_data[startingIndex + 31] << 16 | gps_raw_data[startingIndex + 32] << 8 | gps_raw_data[startingIndex + 33]; 
        lat = gps_raw_data[startingIndex + 34] << 24 | gps_raw_data[startingIndex + 35] << 16 | gps_raw_data[startingIndex + 36] << 8 | gps_raw_data[startingIndex + 37]; 
        //Altitude
        hMSL =  gps_raw_data[startingIndex + 42] << 24 | gps_raw_data[startingIndex + 43] << 16 | gps_raw_data[startingIndex + 44] << 8 | gps_raw_data[startingIndex + 45];
        //Groundspeed
        gSpeed = gps_raw_data[startingIndex + 66] << 24 | gps_raw_data[startingIndex + 67] << 16 | gps_raw_data[startingIndex + 68] << 8 | gps_raw_data[startingIndex + 69];
        heading = gps_raw_data[startingIndex + 70] << 24 | gps_raw_data[startingIndex + 71] << 16 | gps_raw_data[startingIndex + 72] << 8 | gps_raw_data[startingIndex + 73];
        //flags
        fixType = gps_raw_data[startingIndex + 27];
        flags = gps_raw_data[startingIndex + 28];
        additionalFlags = gps_raw_data[startingIndex + 85] << 16 | gps_raw_data[startingIndex + 86] << 8 | gps_raw_data[startingIndex + 87];

        //Accuracy estimates
        hAcc = gps_raw_data[startingIndex + 46] << 24 | gps_raw_data[startingIndex + 47] << 16 | gps_raw_data[startingIndex + 48] << 8 | gps_raw_data[startingIndex + 49];
        vAcc = gps_raw_data[startingIndex + 50] << 24 | gps_raw_data[startingIndex + 51] << 16 | gps_raw_data[startingIndex + 52] << 8 | gps_raw_data[startingIndex + 53];
        sAcc = gps_raw_data[startingIndex + 74] << 24 | gps_raw_data[startingIndex + 75] << 16 | gps_raw_data[startingIndex + 76] << 8 | gps_raw_data[startingIndex + 77];
        headingAcc = gps_raw_data[startingIndex + 78] << 24 | gps_raw_data[startingIndex + 79] << 16 | gps_raw_data[startingIndex + 80] << 8 | gps_raw_data[startingIndex + 81];
    }
    

}

void NEOM8::get_gps_data_error() {
    
}

void NEOM8::calculate_check_sum(uint8_t *check) {

}

void NEOM8::BeginMeasuring() {
    get_gps_data();
}

void NEOM8::GetResult(GpsData_t &Data) {
    Data.dataIsNew = isDataNew;
    isDataNew = false;
    Data.latitude = measuredLatitude;
    Data.longitude = measuredLongitude;
    Data.utcTime = measuredUtcTime;
    Data.groundSpeed = measuredGroundSpeed;
    Data.altitude = measuredAltitude;
    Data.heading = measuredHeading;
    Data.numSatellites = measuredNumSatellites;
}

/*** MAIN CODE ENDS ***/
