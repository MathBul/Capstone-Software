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

// Timer type enum. Note: type B has added functionality compared to type A
typedef enum {
    timer_a, timer_b
} clock_timer_type_t;

// Timer 0 defines
#define TIMER_0A_PERIOD                         23999       // Calculate for steppers to travel 1 foot/sec
#define TIMER_0A_RELOAD_VALUE                   (TIMER_0A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_0A_INTERRUPT_SHIFT                TIMER0A_IRQn

// Timer 1 defines
#define TIMER_1A_PERIOD                         23999
#define TIMER_1A_RELOAD_VALUE                   (TIMER_0A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_1A_INTERRUPT_SHIFT                TIMER1A_IRQn

// Function definitions
void clock_sys_init();
void clock_clear_ifg_raw(TIMER0_Type* timer, clock_timer_type_t type);
void clock_pause_timer(TIMER0_Type* timer, clock_timer_type_t type);
void clock_resume_timer(TIMER0_Type* timer, clock_timer_type_t type);
void clock_set_timer_value(TIMER0_Type* timer, uint16_t value);
void clock_timer0a_init();                           // Steppers
void clock_timer1a_init();                           // Buttons

#endif /* CLOCK_H_ */
