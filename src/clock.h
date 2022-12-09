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

// General clock defines
#define SYSCLOCK_FREQUENCY                      120000000   // 120 MHz

// Timer 0 defines
#define TIMER_0A_PERIOD                         23999       // Calculated for steppers to travel 1 foot/sec
#define TIMER_0A_RELOAD_VALUE                   (TIMER_0A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_0A_INTERRUPT_NUM                  TIMER0A_IRQn

// Timer 1 defines
#define TIMER_1A_PERIOD                         23999       // Calculated for steppers to travel 1 foot/sec
#define TIMER_1A_RELOAD_VALUE                   (TIMER_1A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_1A_INTERRUPT_NUM                  TIMER1A_IRQn

// Timer 2 defines
#define TIMER_2A_PERIOD                         23999       // Calculated for steppers to travel 1 foot/sec
#define TIMER_2A_RELOAD_VALUE                   (TIMER_2A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_2A_INTERRUPT_NUM                  TIMER2A_IRQn

// Timer 3 defines
#define TIMER_3A_PERIOD                         23999       // Chosen through trial-and-error
#define TIMER_3A_RELOAD_VALUE                   (TIMER_3A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_3A_INTERRUPT_NUM                  TIMER3A_IRQn

// Timer 4 defines
#define TIMER_4A_PERIOD                         23999       // Chosen through trial-and-error
#define TIMER_4A_RELOAD_VALUE                   (TIMER_4A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_4A_INTERRUPT_NUM                  TIMER4A_IRQn

// Timer 5 defines
#define TIMER_5A_PERIOD                         119999      // Period: 1s @ 120MHz
#define TIMER_5A_RELOAD_VALUE                   (TIMER_5A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_5A_INTERRUPT_NUM                  TIMER5A_IRQn

// Timer 6 defines
#define TIMER_6A_PERIOD                         23999       // Chosen through trial-and-error
#define TIMER_6A_RELOAD_VALUE                   (TIMER_6A_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_6A_INTERRUPT_NUM                  TIMER6A_IRQn

// Timer 7 defines
#define TIMER_7C_PERIOD                         600000000  // Period: 5s @ 120MHz
#define TIMER_7C_RELOAD_VALUE                   (TIMER_7C_PERIOD << NVIC_ST_RELOAD_S)
#define TIMER_7C_INTERRUPT_NUM                  TIMER7A_IRQn

// Function definitions
void clock_sys_init(void);
void clock_timer0a_init(void);                       // X Stepper
void clock_timer1a_init(void);                       // Y Stepper
void clock_timer2a_init(void);                       // Z Stepper
void clock_timer3a_init(void);                       // Switches
void clock_timer4a_init(void);                       // Gantry
void clock_timer5a_init(void);                       // Delay
void clock_timer6a_init(void);                       // Sensor Network
void clock_timer7c_init(void);                       // Communication Timeout

void clock_clear_interrupt(TIMER0_Type* timer);
void clock_stop_timer(TIMER0_Type* timer);
void clock_start_timer(TIMER0_Type* timer);
void clock_set_timer_period(TIMER0_Type* timer, uint32_t value);
uint32_t clock_get_timer_period(TIMER0_Type* timer);
void clock_reset_timer_value(TIMER0_Type* timer);

#endif /* CLOCK_H_ */
