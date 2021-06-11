#ifndef IMU_INTERFACE_H
#define IMU_INTERFACE_H
/***This interface exists so that freeRTOS can call C-Style functions while not messing up the fact that most of our development is in C++***/

#ifdef __cplusplus
extern "C" {
#endif

void IMUInterfaceExecute(void);


#ifdef __cplusplus
}
#endif

#endif 
