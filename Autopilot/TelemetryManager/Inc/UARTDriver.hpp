#ifndef UARTDRIVER_HPP
#define UARTDRIVER_HPP

#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"FW_CV_Structs.hpp"
#include "usart.h"

void sendFOJI(struct foji msg_to_jetson);
struct fijo decodeFIJO();
bool obtainFIJO();
#endif