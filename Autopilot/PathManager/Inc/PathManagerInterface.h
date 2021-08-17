/**
 * Interface header from path manager (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */


#ifndef PATHMANAGERINTERFACE_H
#define PATHMANAGERINTERFACE_H
/***This interface exists so that freeRTOS can call C-Style functions while not messing up the fact that most of our development is in C++***/

#ifdef __cplusplus
extern "C" {
#endif

bool PathManagerInterfaceExecute(void);


#ifdef __cplusplus
}
#endif

#endif 
