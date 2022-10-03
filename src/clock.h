#include "msp.h"

#define TIMER_0A_PERIOD                         23999
#define TIMER_0A_RELOAD_VALUE                   (TIMER_0A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_0A_INTERRUPT_SHIFT                TIMER0A_IRQn

void sysclock_initialize();
void timer_0a_initialize();
