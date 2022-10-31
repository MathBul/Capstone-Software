/**
 * @file clock.c
 * @author Eli Jelesko (ebj5hec@virginia.edu) and Nick Cooney (npc4crc@virginia.edu)
 * @brief Configures the system clock (120 MHz) and any timers
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#include "clock.h"

/**
 * @brief Configure the system clock to run at 120 MHz using PLL at 240 MHz
 */
void clock_sys_init()
{
    // Configure PLL to be 480 MHz
    SYSCTL->PLLFREQ0 |=  (SYSCTL_PLLFREQ0_PLLPWR);          // Sets PWR to 1
    SYSCTL->PLLFREQ0 |=  (30 << SYSCTL_PLLFREQ0_MINT_S);    // Sets MINT to 30
    SYSCTL->PLLFREQ0 &= ~(1 << SYSCTL_PLLFREQ0_MFRAC_S);    // Sets MFRAC to 0
    SYSCTL->PLLFREQ1 &= ~(1 << SYSCTL_PLLFREQ1_Q_S);        // Sets Q to 0
    SYSCTL->PLLFREQ1 |=  (1 << SYSCTL_PLLFREQ1_N_S);        // Sets N to 1

    // Configure the clock
    SYSCTL->RSCLKCFG |=  (SYSCTL_RSCLKCFG_NEWFREQ);         // Set NEWFREQ to 1
    SYSCTL->RSCLKCFG &= ~(SYSCTL_RSCLKCFG_PLLSRC_PIOSC);    // Use PIOSC for PLL
    SYSCTL->RSCLKCFG &= ~(SYSCTL_RSCLKCFG_OSCSRC_PIOSC);    // Use PIOSC for clock source
    SYSCTL->RSCLKCFG |=  (1 << SYSCTL_RSCLKCFG_PSYSDIV_S);  // Set PSYSDIV to 1

    // Wait until the PLL lock becomes 1 (oscillation is stable), then enable
    while ((SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK) == 0)
    {
    }

    // Configure the memory timings
    SYSCTL->MEMTIM0 |=  (SYSCTL_MEMTIM0_FBCHT_3_5);         // Sets FBCHT to 6
    SYSCTL->MEMTIM0 |=  (SYSCTL_MEMTIM0_EBCHT_3_5);         // Sets EBCHT to 6
    SYSCTL->MEMTIM0 &= ~(SYSCTL_MEMTIM0_FBCE);              // Sets FBCE to 0
    SYSCTL->MEMTIM0 &= ~(SYSCTL_MEMTIM0_EBCE);              // Sets EBCE to 0
    SYSCTL->MEMTIM0 |=  (5 << SYSCTL_MEMTIM0_FWS_S);        // Sets FWS to 5
    SYSCTL->MEMTIM0 |=  (5 << SYSCTL_MEMTIM0_EWS_S);        // Sets EWS to 5

    // Update the memory timings and use PLL
    SYSCTL->RSCLKCFG |= (SYSCTL_RSCLKCFG_MEMTIMU | SYSCTL_RSCLKCFG_USEPLL);
}


/**
 * @brief Configure timer 0A
 */
void clock_timer0a_init()
{
    // Enable the timer and wait for it to be ready
    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
    while (!(SYSCTL->PRTIMER & SYSCTL_RCGCTIMER_R0))
    {
    }

    // Configure Timer 0A for interrupts
    TIMER0->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER0->CFG   =  (0);                                   // Clear the configuration
    TIMER0->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER0->TAILR =  (TIMER_0A_RELOAD_VALUE);               // Set the interval value
    TIMER0->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    NVIC->ISER[0] |= (1 << TIMER_0A_INTERRUPT_SHIFT);
}

/**
 * @brief Configure timer 1A
 */
void clock_timer1a_init()
{
    // Enable the timer and wait for it to be ready
    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R1;
    while (!(SYSCTL->PRTIMER & SYSCTL_RCGCTIMER_R1))
    {
    }

    // Configure Timer 0A for interrupts
    TIMER1->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER1->CFG   =  (0);                                   // Clear the configuration
    TIMER1->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER1->TAILR =  (TIMER_1A_RELOAD_VALUE);               // Set the interval value
    TIMER1->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    NVIC->ISER[0] |= (1 << TIMER_1A_INTERRUPT_SHIFT);
}

/**
 * @brief Configure timer 2A
 */
void clock_timer2a_init()
{
    // Enable the timer and wait for it to be ready
    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R2;
    while (!(SYSCTL->PRTIMER & SYSCTL_RCGCTIMER_R2))
    {
    }

    // Configure Timer 0A for interrupts
    TIMER2->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER2->CFG   =  (0);                                   // Clear the configuration
    TIMER2->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER2->TAILR =  (TIMER_2A_RELOAD_VALUE);               // Set the interval value
    TIMER2->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    NVIC->ISER[0] |= (1 << TIMER_2A_INTERRUPT_SHIFT);
}

/**
 * @brief Clears the interrupt flag associated with time-out raw on the given timer (on the a subsubmodule)
 * 
 * @param timer One of TIMERX for X=0,...,5
 * @param type One of timer_a or timer_b
 */
void clock_clear_interrupt(TIMER0_Type* timer)
{
    timer->ICR |= (TIMER_ICR_TATOCINT);
}

/**
 * @brief Pauses the specified timer (on the a subsubmodule)
 * 
 * @param timer One of TIMERX for X=0,...,5
 * @param type One of timer_a or timer_b
 */
void clock_pause_timer(TIMER0_Type* timer)
{
    timer->CTL &= ~(TIMER_CTL_TAEN);
}

/**
 * @brief Resumes the specified timer (on the a subsubmodule)
 * 
 * @param timer One of TIMERX for X=0,...,5
 * @param type One of timer_a or timer_b
 */
void clock_resume_timer(TIMER0_Type* timer)
{
    timer->CTL |= (TIMER_CTL_TAEN);
}

/**
 * @brief Sets the period of the specified timer
 * 
 * @param timer One of TIMERX for X=0,...,5
 * @param value The period
 */
void clock_set_timer_period(TIMER0_Type* timer, uint16_t value)
{
    timer->CTL  &= ~(TIMER_CTL_TAEN);                       // Disable the timer
    timer->TAILR =  value;                                  // Set the interval value
}

/* End clock.c */
