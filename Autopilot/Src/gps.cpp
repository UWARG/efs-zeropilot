/**
 * Gps sensor
 * Author(s): Dhruv Rawat
 */

#include "gps.hpp"
#include "UART.hpp"
#include "../Libraries/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c"
#include <stddef.h>
#include <string.h>


#define CMS_TO_MPS 0.01f //Converts from cm/s to m/s

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
UARTPort *gpsUART;

char vtgValues[GPS_UART_BUFFER_SIZE];
char ggaValues[GPS_UART_BUFFER_SIZE];
char generalBuffer[GPS_UART_BUFFER_SIZE];
uint8_t tx_buffer[GPS_UART_BUFFER_SIZE];
uint8_t rx_buffer[GPS_UART_BUFFER_SIZE];

bool isGPSConfigured = false;
bool dataAvailable = false;
bool newGGAData = false;
bool newVTGData = false;

void handle_DMA_input();

/*** MAIN CODE BEGINS ***/

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
    
    isGPSConfigured = true;
}

void handle_DMA_input() {
    if (!isGPSConfigured) {
        //Assumed the wrapper will handle clearing the registers and all.
        return;
    }

    bool currentlyParsing = false;
    uint16_t bufferIndex = 0;
    uint8_t data = 0;
    
    /*

        OH MY GOD WHAT AM I SUPPOSED TO DO HERE!?!!?!?!?!?!?!

        NEED TO CHECK IF THERE IS STILL DATA IN THE REGISTER AND ONLY STOP THE 
        WHILE LOOP WHEN THE DATA IS UP!!!!

    */

    while(bufferIndex < 10) { //ADD AN ACTUAL WHILE CONDITION
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

uint8_t NEOM8::uint8_to_hex(uint8_t toConvert) {
    uint8_t intConverted = 0;

    if (toConvert >= 0 && toConvert <= 9) {
        intConverted = toConvert + 0x30;
    } else if (toConvert >= 0xA && toConvert <= 0xF) {
        intConverted = toConvert + 0x37;
    }
    
    return intConverted;
}

uint8_t NEOM8::ascii_to_hex(uint8_t toConvert) {
    uint8_t intConverted = 0;

    if (toConvert == 0x2E) {
        intConverted = 0x10;
    } else if (toConvert >= 0x30 && toConvert <= 0x39) {
        intConverted = toConvert - 0x30;
    } else if (toConvert >= 0x41 && toConvert <= 0x46) {
        intConverted = toConvert - 0x37;
    }

    return intConverted;
}

//Checks if there is new data, validates it, and then converts it to a form that the autopilot will understand
void NEOM8::get_gps_data() {
    if (newGGAData) {
        newGGAData = false;
        if (is_check_sum_valid(ggaValues)) {
            dataAvailable = false;
            parse_gga();
            dataAvailable = true;
        }
    }

    if (newVTGData) {
        newVTGData = false;
        if (is_check_sum_valid(vtgValues)) {
            dataAvailable = false;
            parse_vtg();
            dataAvailable = true;
        }
    }
}

void NEOM8::parse_gga() {
    
}

void NEOM8::parse_vtg() {
    uint8_t raw_heading[6];
    uint8_t raw_ground_speed[8];
    
    //Go through data array and filter out the values we want
    uint16_t commas = 0;
    uint16_t rawDataNavigator = 0;
    uint8_t array_navigation = 0;
    uint8_t raw_data = 0;

    while(vtgValues[rawDataNavigator] != '*') {
        raw_data = ascii_to_hex(vtgValues[rawDataNavigator]);

        if (vtgValues[rawDataNavigator] == ',') {
            commas++;
            array_navigation = 0;
        }

        if (commas == 1 && array_navigation > 0) {
            raw_heading[array_navigation] = raw_data;
        } else if (commas == 7 && array_navigation > 0) {
            raw_ground_speed[array_navigation] = raw_data;
        }

        array_navigation++;
        rawDataNavigator++;
    }

    //Fniish this part

}

void NEOM8::BeginMeasuring() {
    get_gps_data();
    isDataNew = true;
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
