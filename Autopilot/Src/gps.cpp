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
    if(!gps_Instance) {
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
        //Clear the buffer and inturrupt flag and 
        return;
    }

    bool currentlyParsing = false;
    uint16_t bufferIndex = 0;
    uint8_t data = 0;

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

// bool NEOM8::is_check_sum_valid(char *input) {
    
// }

void NEOM8::get_gps_data() {
    
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
