/**
 * Interface header from airspeed driver (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */


#ifndef AIRSPEED_THREAD_INTERFACE_H
#define AIRSPEED_THREAD_INTERFACE_H

#ifdef __cplusplus

extern "C" {
#endif

void airspeedThreadInterfaceInit(void);
void airspeedThreadInterfaceExecute(void);


#ifdef __cplusplus
}
#endif

#endif 
