#include "gpio.h"


#define BUZZER_OFF 0
#define BUZZER_ON 1
#define BUZZER_SHORT_FAST 2
#define BUZZER_SHORT_SLOW 3
#define BUZZER_THREE_SHORT 4
#define BUZZER_LONG 5

#define BUZZER_GPIO_OFF 0
#define BUZZER_GPIO_ON 1


void Buzzer_init();
void Buzzer_Set(uint8_t st);
