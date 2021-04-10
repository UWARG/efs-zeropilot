#pragma once

typedef struct mavlink_custom_cmd_takeoff_t {
    uint8_t beginTakeoff;
} mavlink_custom_cmd_takeoff_t;


#define MAVLINK_MSG_ID_TAKEOFF_CMD 100

#define MAVLINK_MSG_ID_TAKEOFF_CMD_LEN 1
#define MAVLINK_MSG_ID_TAKEOFF_CMD_MIN_LEN 1
#define MAVLINK_MSG_ID_TAKEOFF_CMD_CRC 10
//TODO figure out how to calculate crc or if this crc extra can just be a random number

//typedef struct __mavlink_attitude_t {
 //uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
 //float roll; /*< [rad] Roll angle (-pi..+pi)*/
 //float pitch; /*< [rad] Pitch angle (-pi..+pi)*/
 //float yaw; /*< [rad] Yaw angle (-pi..+pi)*/
 //float rollspeed; /*< [rad/s] Roll angular speed*/
 //float pitchspeed; /*< [rad/s] Pitch angular speed*/
 //float yawspeed; /*< [rad/s] Yaw angular speed*/
//} mavlink_attitude_t;