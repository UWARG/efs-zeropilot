/**
 * Interface header from altimeter driver (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */

#ifndef ALTIMETER_THREAD_INTERFACE_H
#define ALTIMETER_THREAD_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

void AltimeterThreadInterfaceInit(void);
void AltimeterThreadInterfaceExecute(void);


#ifdef __cplusplus
}
#endif

#endif 
