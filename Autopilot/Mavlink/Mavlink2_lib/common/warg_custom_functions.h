#pragma once

typedef struct mavlink_custom_cmd_takeoff_t {
    int beginTakeoff;
} mavlink_custom_cmd_takeoff_t;


#define MAVLINK_MSG_ID_TAKEOFF_CMD 100

#define MAVLINK_MSG_ID_TAKEOFF_CMD_LEN 12
#define MAVLINK_MSG_ID_TAKEOFF_CMD_MIN_LEN 12
#define MAVLINK_MSG_ID_TAKEOFF_CMD_CRC 50
//TODO figure out how to calculate crc or if this crc extra can just be a random number