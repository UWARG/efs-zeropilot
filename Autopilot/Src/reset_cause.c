/*
#define LPWRRSTF_Bit 31
#define WWDGRSTF_Bit 30
#define IWDGRSTF_Bit 29
#define SFTRSTF_Bit 28
#define PORRSTF_Bit 27
#define PINRSTF_Bit 26 
#define OBLRSTF_Bit 25 //Option Byte Loader
*/

#include "debug.h"
#include "stm32f7xx_hal_rcc.h"
/*
void reset_cause_check(){
    uint* RCC_CSR = 0x24;
    if(*RCC_CSR & (1 << LPWRRSTF_Bit) == (1 << LPWRRSTF_Bit)){
        debug("Lower power reset");
    }
    else if(*RCC_CSR & (1 << WWDGRSTF_Bit) == (1 << WWDGRSTF_Bit)){
        debug("Window watchdog reset");
    }

*/

void reset_cause_check(){
    if(_HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) debug("Pin reset");
    else if(_HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) debug("POR/PWD reset");
    else if(_HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) debug("Software reset");
    else if(_HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) debug("Independent rest");
    else if(_HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) debug("Watch dog reset");
    else if(_HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) debug("Low power reset");
}