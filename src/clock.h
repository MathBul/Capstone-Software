/**
 * @file clock.h
 * @author Eli Jelesko (ebj5hec@virginia.edu) and Nick Cooney (npc4crc@virginia.edu)
 * @brief Configures the system clock (120 MHz) and any timers
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "msp.h"
#include "utils.h"

// Timer 0 defines
#define TIMER_0A_PERIOD                         23999       // Calculate for steppers to travel 1 foot/sec
#define TIMER_0A_RELOAD_VALUE                   (TIMER_0A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_0A_INTERRUPT_SHIFT                TIMER0A_IRQn

// Timer 1 defines
#define TIMER_1A_PERIOD                         23999       // TODO: Calculate
#define TIMER_1A_RELOAD_VALUE                   (TIMER_1A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_1A_INTERRUPT_SHIFT                TIMER1A_IRQn

// Timer 2 defines
#define TIMER_2A_PERIOD                         23999       // TODO: Calculate
#define TIMER_2A_RELOAD_VALUE                   (TIMER_2A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_2A_INTERRUPT_SHIFT                TIMER2A_IRQn

// Timer 3 defines
#define TIMER_3A_PERIOD                         23999       // TODO: Calculate
#define TIMER_3A_RELOAD_VALUE                   (TIMER_3A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_3A_INTERRUPT_SHIFT                TIMER3A_IRQn

// Timer 4 defines
#define TIMER_4A_PERIOD                         23999       // TODO: Calculate
#define TIMER_4A_RELOAD_VALUE                   (TIMER_4A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_4A_INTERRUPT_SHIFT                TIMER4A_IRQn

// Timer 5 defines
#define TIMER_5A_PERIOD                         23999       // TODO: Calculate
#define TIMER_5A_RELOAD_VALUE                   (TIMER_5A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_5A_INTERRUPT_SHIFT                TIMER5A_IRQn


// Function definitions
void clock_sys_init();
void clock_timer0a_init();                           // X stepper
void clock_timer1a_init();                           // Y stepper
void clock_timer2a_init();                           // Z stepper
void clock_timer3a_init();                           // switches
void clock_timer4a_init();                           // Gantry
void clock_timer5a_init();                           // Delay

void clock_clear_interrupt(TIMER0_Type* timer);
void clock_pause_timer(TIMER0_Type* timer);
void clock_resume_timer(TIMER0_Type* timer);
void clock_set_timer_period(TIMER0_Type* timer, uint16_t value);

#endif /* CLOCK_H_ */
