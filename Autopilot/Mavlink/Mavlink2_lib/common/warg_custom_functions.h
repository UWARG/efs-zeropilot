/**************************************************************************************************/
// Author: Jingting Liu
// April 9th, 2021

// this file is created for the purpose of supporting custom WARG Mavlink messages

// The decoder and encoder only support GPS and gimbal control, other simpler commands will be taken
// care of by the Xbee communication directly.

// checkout this thread to view the issue/reasoning: //https://github.com/mavlink/mavlink/issues/1092
/**************************************************************************************************/
#pragma once

typedef struct mavlink_custom_cmd_takeoff_t {
    uint8_t beginTakeoff;
} mavlink_custom_cmd_takeoff_t;


#define MAVLINK_MSG_ID_TAKEOFF_CMD 100

#define MAVLINK_MSG_ID_TAKEOFF_CMD_LEN 1
#define MAVLINK_MSG_ID_TAKEOFF_CMD_MIN_LEN 1
#define MAVLINK_MSG_ID_TAKEOFF_CMD_CRC 17521 
