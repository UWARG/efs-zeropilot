/**
 * Gps sensor
 * Author(s): Dhruv Rawat
 */

#include "gps.hpp"
#include <stddef.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "UART.hpp"

#define KTS_TO_MPS 0.51444f //Converts from cm/s to m/s

#define GPS_UART_BAUDRATE 9600
#define GPS_UART_BUFFER_SIZE 100

#define PUBX_CONFIG_NMEA "$PUBX,41,1,07,03,9600,0*25\r\n" //Change hex numbers later
#define PUBX_SET_GGA "$PUBX,40,GGA,0,1,0,0,0,0*5D\r\n"
#define PUBX_SET_VTG "$PUBX,40,VTG,0,1,0,0,0,0*5D\r\n"
#define PUBX_SET_GGA_OFF "$PUBX,40,GGA,0,0,0,0,0,0*5D\r\n"
#define PUBX_SET_VTG_OFF "$PUBX,40,VTG,0,0,0,0,0,0*5D\r\n"

#define GGA_MESSAGE "GPGGA"
#define VTG_MESSAGE "GPVTG"

NEOM8* NEOM8::gps_Instance = NULL;
static UARTPort *gpsUART;

//Data buffers
static char vtgValues[GPS_UART_BUFFER_SIZE];
static char ggaValues[GPS_UART_BUFFER_SIZE];
static char generalBuffer[GPS_UART_BUFFER_SIZE];
static uint8_t tx_buffer[GPS_UART_BUFFER_SIZE];
static uint8_t rx_buffer[GPS_UART_BUFFER_SIZE];

static bool newGGAData = false;
static bool newVTGData = false;
static bool parsingForCollection = false; 

/*** MAIN CODE BEGINS ***/

static void handle_DMA_input() {
    if (parsingForCollection) { //If we are parsing data, we don't want the program writing over the static data registers in this file.
        return;
    }

    bool currentlyParsing = false;
    uint16_t bufferIndex = 0;
    uint8_t data = 0;

    /*
        Without this, I don't have anything concrete to define the condition of the for loop
        and copy data to the static data registers in this file. 
    */

    std::deque<uint8_t> dma_data = gpsUART->get_rx_queue(); //Copys double eneded queue in UARTPort class that contains data in the DMA registers

    for (int i = 0; i < sizeof(dma_data) && newGGAData == false && newVTGData == false; i++) { // If new data is found, then the loop is terminated 
        data = dma_data[i];

        if (data == '$') {
            currentlyParsing = true;
            bufferIndex = 0;
        } else if (data == '\r') {
            if (strncmp(GGA_MESSAGE, generalBuffer, 5 == 0)) {
                memcpy(ggaValues, generalBuffer, GPS_UART_BUFFER_SIZE);
                newGGAData = true;
            } else if (strncmp(VTG_MESSAGE, generalBuffer, 5 == 0)) {
                memcpy(vtgValues, generalBuffer, GPS_UART_BUFFER_SIZE);
                newVTGData = true;
            } 
        } else if (currentlyParsing) {
            generalBuffer[bufferIndex] = data;
            bufferIndex = (bufferIndex + 1) % GPS_UART_BUFFER_SIZE;
        }
    }
}

NEOM8* NEOM8::GetInstance() {
    if (!gps_Instance) {
        gps_Instance = new NEOM8;
    }
    return gps_Instance;
}

NEOM8::NEOM8() {
    //Configures UART interface
    UARTSettings gpsUARTConfig;
    gpsUARTConfig.parity = UART_NO_PARITY;
    gpsUARTConfig.baudrate = 115200;

    gpsUART = new UARTPort(UART_PORT1, gpsUARTConfig); //Confirm values later
    gpsUART->reset();
    gpsUART->setup();

    gpsUART->resetDMA();
    gpsUART->setupDMA(sizeof(tx_buffer), sizeof(rx_buffer)); //Sets up 100 byte rx and tx buffers in DMA

    gpsUART->registerDMAReceiveCallback(handle_DMA_input);
    //GPS Configuration
    gpsUART->transmit((uint8_t*) PUBX_CONFIG_NMEA, sizeof(PUBX_CONFIG_NMEA));
    //Turns off receivers (resets them)
    gpsUART->transmit((uint8_t*) PUBX_SET_GGA_OFF, sizeof(PUBX_SET_GGA_OFF));
    gpsUART->transmit((uint8_t*) PUBX_SET_VTG_OFF, sizeof(PUBX_SET_VTG_OFF));
    //Turns on receivers
    gpsUART->transmit((uint8_t*) PUBX_SET_GGA, sizeof(PUBX_SET_GGA));
    gpsUART->transmit((uint8_t*) PUBX_SET_VTG, sizeof(PUBX_SET_VTG));
    
}

//Checks if there is new data, validates it, and then converts it to a form that the autopilot will understand
void NEOM8::parse_gps_data() {
    if (newGGAData) {
        newGGAData = false;
        if (is_check_sum_valid(ggaValues)) {
            dataAvailable = false;
            parse_gga();
            dataAvailable = true;
            isDataNew = true;
            ggaDataNew = true;
        }
    }

    if (newVTGData) {
        newVTGData = false;
        if (is_check_sum_valid(vtgValues)) {
            dataAvailable = false;
            parse_vtg();
            dataAvailable = true;
            isDataNew = true;
            vtgDataNew = true;
        }
    }
}

void NEOM8::parse_vtg() {
    int raw_heading[5] = {0, 0, 0, 0, 0};
    int raw_ground_speed[7] = {0, 0, 0, 0, 0, 0, 0};

    //Go through data array and filter out the values we want
    int commas = 0;
    int rawDataNavigator = 0;
    int array_navigation = 0;
    int raw_data = 0;

    while(vtgValues[rawDataNavigator] != '*') {
        raw_data = ascii_to_hex(vtgValues[rawDataNavigator]);
        while (vtgValues[rawDataNavigator] == ',' && ggaValues[rawDataNavigator + 1] != '*') { //Commas divide different data
            commas++;
            array_navigation = 0;
            rawDataNavigator++;
            raw_data = ascii_to_hex(vtgValues[rawDataNavigator]);
        }

        if (commas == 1) { //If heading data is being displayed
            raw_heading[array_navigation] = raw_data;
        } else if (commas == 7) {//If speed is being displayed
            raw_ground_speed[array_navigation] = raw_data;
        }

        array_navigation++;
        rawDataNavigator++;
    }

    array_navigation = 0;
    long int multiplier = 10;
    int decimalPoint = 0;

    // Converts Heading data (Measured in Degrees)
    measuredHeading = 0;
    float tempHeading = 0.0f;

    for (int i = 0; i < 5; ++i) {
        if(raw_heading[i] == 46) { //Decimal point
            decimalPoint = i;
        } else { //We get the digits of the heading. This ensures that the digit is in the correct place (tens, ones, tenths, etc.)
            tempHeading += (float) (raw_heading[i]*100000 / multiplier);
            multiplier *= 10;
        }
    }

    decimalPoint -= 2;
    multiplier = 10000;

    while (decimalPoint >= 0) { //Gets the multiplier that will place the decimal point in the correct place.
        multiplier /= 10;
        decimalPoint--;
    }

    measuredHeading = (int) (tempHeading / multiplier)%360;

    //Converts speed data (Measured in m/s)
    array_navigation = 0;
    multiplier = 10;
    decimalPoint = 0;

    measuredGroundSpeed = 0;

    for(int i = 0; i < 7; i++) {
        if(raw_ground_speed[i] == 46) {
            decimalPoint = i;
        } else {
            measuredGroundSpeed += (float) (raw_ground_speed[i] * 1000000 / multiplier);
            multiplier *= 10;
        }
    }

    decimalPoint = decimalPoint -2;
    multiplier = 100000;

    while(decimalPoint >= 0) {
        multiplier /= 10;
        decimalPoint--;
    }

    measuredGroundSpeed /= multiplier;
}

void NEOM8::parse_gga() {
    int raw_time[10];
    int raw_lat[10];
    int raw_long[10];
    int raw_satellites[2] = {0, 10};
    uint8_t raw_altitude[7];
    int latitude_direction = 0; //North or South
    int longitude_direction = 0; //East or west

    //Parse through raw data and extract relevant info
    int commas = 0;
    int array_navigation = 0;
    int rawDataNavigator = 0;
    int raw_data = 0;

    while (ggaValues[rawDataNavigator] != '*') {
        raw_data = ascii_to_hex(ggaValues[rawDataNavigator]);

        while (ggaValues[rawDataNavigator] == ',' && ggaValues[rawDataNavigator + 1] != '*') { //Commas divide different data
            commas++;
            array_navigation = 0;
            rawDataNavigator++;
            raw_data = ascii_to_hex(ggaValues[rawDataNavigator]);
        }

        if (commas == 1) {
            raw_time[array_navigation] = raw_data;
        } else if (commas == 2) {
            raw_lat[array_navigation] = raw_data;
        } else if (commas == 3) {
            latitude_direction = raw_data;
        } else if (commas == 4) {
            raw_long[array_navigation] = raw_data;
        } else if (commas == 5) {
            longitude_direction = raw_data;
        } else if (commas == 6) {
            gpsFixStatus = raw_data;
        } else if (commas == 7) {
            raw_satellites[array_navigation] = raw_data;
        } else if (commas == 9) {
            raw_altitude[array_navigation] = raw_data;
        }

        array_navigation++;
        rawDataNavigator++;
    }

    //Determine Fix status
    if (gpsFixStatus == 6) { //Estimated/Dead Reckoning fix
        gpsFixStatus = 3;
    } else if (gpsFixStatus == 5 || gpsFixStatus == 4) { //Gps fix
        gpsFixStatus = 1;
    } else if (gpsFixStatus == 1 || gpsFixStatus == 2) { //DGSP fix
        gpsFixStatus = 2;
    }

    //Calclate time (Seconds since midnight)
    measuredUtcTime = (float) raw_time[0] * 100000;
    measuredUtcTime += (float) raw_time[1] * 10000;
    measuredUtcTime += (float) raw_time[2] * 1000;
    measuredUtcTime += (float) raw_time[3] * 100;
    measuredUtcTime += (float) raw_time[4] * 10;
    measuredUtcTime += (float) raw_time[5] * 1;
    //Decimal Point
    measuredUtcTime += (float) raw_time[6] * 0.1;
    measuredUtcTime += (float) raw_time[7] * 0.01;
    measuredUtcTime += (float) raw_time[8] * 0.001;
    measuredUtcTime = (int) measuredUtcTime%86400; //Seconds past midnight

    //Calculate latitude
    measuredLatitude = raw_lat[2]*10.0;
    measuredLatitude += raw_lat[3]*1.0;
    measuredLatitude += raw_lat[5]*0.1;
    measuredLatitude += raw_lat[6]*0.01;
    measuredLatitude += raw_lat[7]*0.001;
    measuredLatitude += raw_lat[8]*0.0001;
    measuredLatitude /= 60;  //Converts from dd.mmmmmm to decimal degrees. (60 minutes in a degree)
    //Then add the degrees (ranges from -90 to +90)
    measuredLatitude += raw_lat[0]*10.0;
    measuredLatitude += raw_lat[1]*1.0;

    if (latitude_direction == 28) { //S
        measuredLatitude *= -1;
    }

    //Calculate longitude
    measuredLongitude = raw_long[3]*10.0;
    measuredLongitude += raw_long[4]*1.0;
    measuredLongitude += raw_long[6]*0.1;
    measuredLongitude += raw_long[7]*0.01;
    measuredLongitude += raw_long[8]*0.001;
    measuredLongitude += raw_long[9]*0.0001;
    measuredLongitude /= 60;  //Converts from ddd.mmmmmm to decimal degrees. (60 minutes in a degree)
    //Then add the degrees (ranges from -180 to +180)
    measuredLongitude += raw_long[0]*100.0;
    measuredLongitude += raw_long[1]*10.0;
    measuredLongitude += raw_long[2]*1.0;

    if (longitude_direction == 32) { //W
        measuredLongitude *= -1;
    }

    //calculate satellites
    if (raw_satellites[1] == 10) {
        measuredNumSatellites = raw_satellites[0];
    } else {
        measuredNumSatellites = raw_satellites[0] * 10 + raw_satellites[1];
    }

    //calculate altitude - tricky because of unknown 1-3 digits preceeding the decimal
    long int multiplier = 10;
    int decimalPoint = 0;
    measuredAltitude = 0;
    float tempAltitude = 0;

    for (int i = 0; i < 7; i++) { //this code first generates an 6 digit decimal number
        if (raw_altitude[i] == 46) //check for decimal point
        {
            decimalPoint = i;
        } else {
            tempAltitude += (float) (raw_altitude[i] * 1000000 / multiplier);
            multiplier *= 10;
        }
    }

    decimalPoint = decimalPoint - 2;
    multiplier = 100000;

    while (decimalPoint >= 0) {
        multiplier /= 10;
        decimalPoint--;
    }

    measuredAltitude = (int) (tempAltitude / multiplier);
}

bool NEOM8::is_check_sum_valid(char *input) {
    uint16_t index = 0;
    uint8_t checksum = 0;

    while(input[index] != '*') {
        checksum ^= input[index];
        index++;
    }

    index++;
    return uint8_to_hex((checksum & 0xF0) >> 4) == input[index] && uint8_to_hex(checksum & 0x0F) == input[index+1];
}

uint8_t NEOM8::uint8_to_hex(unsigned int toConvert) {
    uint8_t intConverted = 0;

    if (toConvert >= 0 && toConvert <= 9) {
        intConverted = toConvert + 0x30;
    } else if (toConvert >= 0xA && toConvert <= 0xF) {
        intConverted = toConvert + 0x37;
    }
    
    return intConverted;
}

int NEOM8::ascii_to_hex(unsigned int toConvert) {
    int intConverted = 0;

    if (toConvert == 46) {
        intConverted = 46;
    } else if (toConvert >= 48 && toConvert <= 57) {
        intConverted = toConvert - 48;
    } else if (toConvert >= 65 && toConvert <= 70) {
        intConverted = toConvert - 0x37;
    }

    return intConverted;
}

void NEOM8::GetResult(GpsData_t &Data) {
    parsingForCollection = true;
    parse_gps_data(); 

    Data.dataIsNew = isDataNew;
    Data.ggaDataIsNew = ggaDataNew;
    Data.vtgDataIsNew = vtgDataNew;
    vtgDataNew = false;
    ggaDataNew = false;
    isDataNew = false;
    Data.latitude = measuredLatitude;
    Data.longitude = measuredLongitude;
    Data.utcTime = measuredUtcTime;
    Data.groundSpeed = measuredGroundSpeed;
    Data.altitude = measuredAltitude;
    Data.heading = measuredHeading;
    Data.numSatellites = measuredNumSatellites;
    Data.fixStatus = gpsFixStatus;

    parsingForCollection = false;
}

/*** MAIN CODE ENDS ***/
