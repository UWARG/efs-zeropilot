/**
 * @file TelemetryManager.c
 * @author Jingting Liu
 * @date January 14th, 2021
 * @copyright Waterloo Aerial Robotics Group 2020-2021 \n
 */


#ifndef _TELEMETRY_STATE_MACHINE_H_
#define _TELEMETRY_STATE_MACHINE_H_


typedef enum TM_STATE_e {
    INIT,
    WAIT_FOR_GROUND_DATA,
    CONNECT_WITH_PATH_MANAGER,
    REGULAR_REPORT_READY,
    ERR_REPORT_READY,
    SEND_ERR_REPORT,
    SEND_REGULAR_REPORT,
} TM_STATE_e;

void telemetry_manager(void);
#endif