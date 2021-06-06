#ifndef TELEMETRYMANAGERINTERFACE_H
#define TELEMETRYMANAGERINTERFACE_H
/***This interface exists so that freeRTOS can call C-Style functions while not messing up the fact that most of our development is in C++***/

#ifdef __cplusplus
extern "C" {
#endif

bool TelemetryManagerInterfaceExecute(void);


#ifdef __cplusplus
}
#endif

#endif 