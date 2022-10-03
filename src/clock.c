#include "clock.h"

/*
 * Configure the system clock to run at 120 MHz using PLL at 240 MHz
 */
void sysclock_initialize()
{
    // Configure PLL to be 480 MHz
    SYSCTL->PLLFREQ0 |= (SYSCTL_PLLFREQ0_PLLPWR);           // Sets PWR to 1
    SYSCTL->PLLFREQ0 |= (30 << SYSCTL_PLLFREQ0_MINT_S);     // Sets MINT to 30
    SYSCTL->PLLFREQ0 &= ~(1 << SYSCTL_PLLFREQ0_MFRAC_S);    // Sets MFRAC to 0
    SYSCTL->PLLFREQ1 &= ~(1 << SYSCTL_PLLFREQ1_Q_S);        // Sets Q to 0
    SYSCTL->PLLFREQ1 |= (1 << SYSCTL_PLLFREQ1_N_S);         // Sets N to 1

    // Configure the clock
    SYSCTL->RSCLKCFG |= (SYSCTL_RSCLKCFG_NEWFREQ);          // Set NEWFREQ to 1
    SYSCTL->RSCLKCFG &= ~(SYSCTL_RSCLKCFG_PLLSRC_PIOSC);    // Use PIOSC for PLL
    SYSCTL->RSCLKCFG &= ~(SYSCTL_RSCLKCFG_OSCSRC_PIOSC);    // Use PIOSC for clock source
    SYSCTL->RSCLKCFG |= (1 << SYSCTL_RSCLKCFG_PSYSDIV_S);   // Set PSYSDIV to 1

    // Wait until the PLL lock becomes 1 (oscillation is stable), then enable
    while ((SYSCTL->PLLSTAT & SYSCTL_PLLSTAT_LOCK) == 0)
    {
    }

    // Configure the memory timings
    SYSCTL->MEMTIM0 |= (SYSCTL_MEMTIM0_FBCHT_3_5);          // Sets FBCHT to 6
    SYSCTL->MEMTIM0 |= (SYSCTL_MEMTIM0_EBCHT_3_5);          // Sets EBCHT to 6
    SYSCTL->MEMTIM0 &= ~(SYSCTL_MEMTIM0_FBCE);              // Sets FBCE to 0
    SYSCTL->MEMTIM0 &= ~(SYSCTL_MEMTIM0_EBCE);              // Sets EBCE to 0
    SYSCTL->MEMTIM0 |= (5 << SYSCTL_MEMTIM0_FWS_S);         // Sets FWS to 5
    SYSCTL->MEMTIM0 |= (5 << SYSCTL_MEMTIM0_EWS_S);         // Sets EWS to 5

    // Update the memory timings and use PLL
    SYSCTL->RSCLKCFG |= (SYSCTL_RSCLKCFG_MEMTIMU | SYSCTL_RSCLKCFG_USEPLL);
}

/*
 * Configure timer 0A
 */
void timer_0a_initialize()
{
    // Enable the timer and wait for it to be ready
    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
    while (!(SYSCTL->PRTIMER & SYSCTL_RCGCTIMER_R0))
    {
    }

    // Configure Timer 0A for interrupts
    TIMER0->CTL  &= ~(TIMER_CTL_TAEN);                      // Disable the timer
    TIMER0->CFG   = (0);                                    // Clear the configuration
    TIMER0->TAMR  = (TIMER_TAMR_TAMR_PERIOD);               // Configure for periodic interrupts
    TIMER0->TAILR = (TIMER_0A_RELOAD_VALUE);                // Set the interval value
    TIMER0->IMR  |= (TIMER_IMR_TATOIM);                     // Set the interrupt mask
    TIMER0->CTL  |= (TIMER_CTL_TAEN);                       // Enable the timer

    // Configure the interrupt in the NVIC
    NVIC->ISER[0] |= (1 << TIMER_0A_INTERRUPT_SHIFT);       // Enable the interrupt in the NVIC
}

// End clock.c
