/**
 * Battery Sensor Header File
 * 
 * This will basically sense the voltage of the battery.
 * 
 * Author: Sahil Kale
 *  
 **/

#ifndef BATTERYSENSOR_HPP
#define BATTERYSENSOR_HPP

/**
 * Starts the ADC for the battery sensor 
 */
void initBatterySensor();

/**
 *  Returns the battery voltage as a float
 * 
 * @param battery The selected battery to return the voltage from
 */
float getBatteryVoltage(int battery);

#endif