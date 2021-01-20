/**
 * @file Telemetry_Manager.c
 * @author Jingting Liu
 * @date January 14th, 2021
 */

#include <stdio.h>
#include <common/mavlink.h>

#include "telemetry_state_machine.h"



TM_STATE_e current_state = INIT;

// TODO: Add FreeRTOS specifics, return type, waits etc
void telemetery_manager()
{
    while (1)
    {
        switch(current_state)
        {
            case INIT:
            {
                // TODO: Insert init code
                current_state = CONNECT_WITH_PATH_MANAGER;
                break;
            }
            
            case WAIT_FOR_GROUND_DATA:
            {
                while (! (serial.bytesAvailable > 0))
                {
                    // TODO: FreeRTOS vTaskDelay for x us/ms/whatever: https://www.freertos.org/a00127.html
                }
                // received data - mostly encoded GPS coordinate
                uint8_t byte = serial.getNextByte();

                //TODO: insert data decoder

                //TODO analysis if data is valid
                bool isValidData = true;
                if (isValidData)
                {
                    current_state = CONNECT_WITH_PATH_MANAGER;
                }
                break;
            }

            case CONNECT_WITH_PATH_MANAGER:
            {
                // TODO: send data to Path manager

                // TODO: receive feedback data from path manager, use an empty while loop to wait for data,
                bool isValidData = true;
                if (isValidData)
                {
                    current_state = REGULAR_REPORT_READY;
                }
                else
                {
                    // case of receiving error or empty data after a certain time
                    current_state = ERR_REPORT_READY;
                }
                break;
            }

            case REGULAR_REPORT_READY:
            {
                // TODO: encode data using Mavlink
                
                current_state = SEND_REGULAR_REPORT;
                break;
            }

            case ERR_REPORT_READY:
            {
                // TODO: encode data using Mavlink
                
                current_state = SEND_ERR_REPORT;
                break;
            }

            case SEND_REGULAR_REPORT:
            {
                // TODO: send data back to ground

                current_state = WAIT_FOR_GROUND_DATA;
                break;
            }

            case SEND_ERR_REPORT:
            {
                // TODO: send data back to ground

                current_state = INIT;
                break;
            }

            default:
                printf("Invalid telemetry manager state: This should never happen\r\n");
        }
    }
}

int main()
{

}
