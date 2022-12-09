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
void clock_sys_init(void)
{
    // Configure PLL to be 480 MHz
    SYSCTL->PLLFREQ0 |=  (SYSCTL_PLLFREQ0_PLLPWR);          // Sets PWR to 1
    SYSCTL->PLLFREQ0 |=  (30 << SYSCTL_PLLFREQ0_MINT_S);    // Sets MINT to 30
    SYSCTL->PLLFREQ0 &= ~(1  << SYSCTL_PLLFREQ0_MFRAC_S);   // Sets MFRAC to 0
    SYSCTL->PLLFREQ1 &= ~(1  << SYSCTL_PLLFREQ1_Q_S);       // Sets Q to 0
    SYSCTL->PLLFREQ1 |=  (1  << SYSCTL_PLLFREQ1_N_S);       // Sets N to 1

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
void clock_timer0a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER0);

    // Configure the timer for interrupts
    TIMER0->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER0->CFG   =  (0);                                   // Clear the configuration
    TIMER0->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER0->TAILR =  (TIMER_0A_RELOAD_VALUE);               // Set the interval value
    TIMER0->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_0A_INTERRUPT_NUM, 5);
}

/**
 * @brief Configure timer 1A
 */
void clock_timer1a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER1);

    // Configure the timer for interrupts
    TIMER1->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER1->CFG   =  (0);                                   // Clear the configuration
    TIMER1->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER1->TAILR =  (TIMER_1A_RELOAD_VALUE);               // Set the interval value
    TIMER1->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_1A_INTERRUPT_NUM, 5);
}

/**
 * @brief Configure timer 2A
 */
void clock_timer2a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER2);

    // Configure the timer for interrupts
    TIMER2->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER2->CFG   =  (0);                                   // Clear the configuration
    TIMER2->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER2->TAILR =  (TIMER_2A_RELOAD_VALUE);               // Set the interval value
    TIMER2->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_2A_INTERRUPT_NUM, 5);
}

/**
 * @brief Configure timer 3A
 */
void clock_timer3a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER3);

    // Configure the timer for interrupts
    TIMER3->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER3->CFG   =  (0);                                   // Clear the configuration
    TIMER3->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER3->TAILR =  (TIMER_3A_RELOAD_VALUE);               // Set the interval value
    TIMER3->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_3A_INTERRUPT_NUM, 1);
}

/**
 * @brief Configure timer 4A
 */
void clock_timer4a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER4);

    // Configure the timer for interrupts
    TIMER4->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER4->CFG   =  (0);                                   // Clear the configuration
    TIMER4->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER4->TAILR =  (TIMER_4A_RELOAD_VALUE);               // Set the interval value
    TIMER4->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_4A_INTERRUPT_NUM, 2);
}

/**
 * @brief Configure timer 5A
 */
void clock_timer5a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER5);

    // Configure the timer for interrupts
    TIMER5->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER5->CFG   =  (0);                                   // Clear the configuration
    TIMER5->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER5->TAILR =  (TIMER_5A_RELOAD_VALUE);               // Set the interval value
    TIMER5->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_5A_INTERRUPT_NUM, 5);
}

/**
 * @brief Configure timer 6A
 */
void clock_timer6a_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER6);

    // Configure the timer for interrupts
    TIMER6->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER6->CFG   =  (0);                                   // Clear the configuration
    TIMER6->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER6->TAILR =  (TIMER_6A_RELOAD_VALUE);               // Set the interval value
    TIMER6->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_6A_INTERRUPT_NUM, 5);
}

/**
 * @brief Configure timer 7A
 */
void clock_timer7c_init(void)
{
    // Enable the timer and wait for it to be ready
    utils_timer_clock_enable(TIMER7);

    // Configure the timer for interrupts
    TIMER7->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER7->CFG   =  (0);
    TIMER7->TAMR  =  (TIMER_TAMR_TAMR_PERIOD);              // Configure for periodic interrupts
    TIMER7->TAILR =  (TIMER_7C_RELOAD_VALUE);               // Set the interval value
    TIMER7->IMR  |=  (TIMER_IMR_TATOIM);                    // Set the interrupt mask

    // Configure the interrupt in the NVIC
    utils_set_nvic(TIMER_7C_INTERRUPT_NUM, 5);
}

/**
 * @brief Clears the interrupt flag associated with time-out on the given timer (on the A subsubmodule)
 * 
 * @param timer One of TIMERX for X={0,...,7}
 * @param type One of timer_a or timer_b
 */
void clock_clear_interrupt(TIMER0_Type* timer)
{
    timer->ICR |= (TIMER_ICR_TATOCINT);
}

/**
 * @brief Pauses the specified timer (on the A subsubmodule)
 * 
 * @param timer One of TIMERX for X={0,...,7}
 */
void clock_stop_timer(TIMER0_Type* timer)
{
    timer->CTL &= ~(TIMER_CTL_TAEN);
}

/**
 * @brief Resumes the specified timer (on the A subsubmodule)
 * 
 * @param timer One of TIMERX for X={0,...,7}
 */
void clock_start_timer(TIMER0_Type* timer)
{
    timer->CTL |= (TIMER_CTL_TAEN);
}

/**
 * @brief Sets the period of the specified timer
 * 
 * @param timer One of TIMERX for X={0,...,7}
 * @param value The period
 */
void clock_set_timer_period(TIMER0_Type* timer, uint32_t value)
{
    timer->CTL  &= ~(TIMER_CTL_TAEN);                       // Disable the timer
    timer->TAILR =  value;                                  // Set the interval value
}

/**
 * @brief Gets the period of the specified timer
 *
 * @param timer One of TIMERX for X={0,...,7}
 * @return The period
 */
uint32_t clock_get_timer_period(TIMER0_Type* timer)
{
    uint32_t value;

    timer->CTL  &= ~(TIMER_CTL_TAEN);                       // Disable the timer
    value = (uint32_t) timer->TAILR;                        // Get the interval value
    timer->CTL |= (TIMER_CTL_TAEN);                         // Enable the timer

    return value;
}

/**
 * @brief Resets the period of the specified timer
 *
 * @param timer One of TIMERX for X={0,...,7}
 */
void clock_reset_timer_value(TIMER0_Type* timer)
{
    timer->CTL &= ~(TIMER_CTL_TAEN);                        // Disable the timer
    timer->TAV  = clock_get_timer_period(timer);            // Reset the value to the period
}

/* End clock.c */
