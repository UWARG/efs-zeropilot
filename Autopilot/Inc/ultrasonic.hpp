/**
 * Ultrasonic Sensor Functions
 * Author: Dhruv Upadhyay, February 2021
 */

 #ifndef ULTRASONIC_HPP
 #define ULTRASONIC_HPP

 struct ultrasonicData_t {
    float distance;
    bool isDataNew; // Holds whether the sensor data has been updated or not
    
 }