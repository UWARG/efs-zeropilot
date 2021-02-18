/**
 * Ultrasonic Sensor Functions
 * Author: Dhruv Upadhyay, February 2021
 */

 #ifndef ULTRASONIC_HPP
 #define ULTRASONIC_HPP

 #include <stdint.h>


const int SOUND_SPEED = 343; // In meters per second, used to calculate distance based on time taken to send and receive ping

typedef struct {
    float distance;
    bool isDataNew; // Holds whether the sensor data has been updated or not
    enum status {FAILING=-1, SENT=0, RECEIVED=1}; // -1 for trig not sent and echo not received, 0 for trig sent, echo not received, and 1 for echo received
 } ultrasonicData_t;


class ultrasonic {
    public:

        // Initializes the data variables 
        virtual void Init(void) = 0;

        // Starts the process of sending and receiving ultrasonic signals and holds the values in variables
        virtual void beginMeasuring(void);

        // Used to update the values in the struct from the values received from the beginMeasuring() function
        // Resets the status of the ultrasonic sensor
        virtual void getData(ultrasonicData_t * Data);
}

#endif