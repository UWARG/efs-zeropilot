
#ifndef ATTITUDEMANAGERINTERFACE_H
#define ATTITUDEMANAGERINTERFACE_H
// This interface exists so that freeRTOS can call C-Style functions while not messing up the fact that most of our development is in C++

#ifdef __cplusplus
extern "C" {
#endif

void AttitudeManagerInterfaceExecute(void);


#ifdef __cplusplus
}
#endif 

#endif