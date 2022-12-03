/**
 * @file pwm.c
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Code for using PWM on the MSP432E401Y
 * @version 0.1
 * @date 2022-12-02
 *
 * @copyright Copyright (c) 2022
 */

#include "pwm.h"

void pwm_init()
{
    // Enable the PWM clock
    SYSCTL->RCGCPWM |= SYSCTL_RCGCPWM_R0;

    // Enable the GPIO clock
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R9;

    // Enable PWM as alternate function on PK4 and PK5
    gpio_select_alternate_function(GPIOK, GPIO_PIN_4, 6);
    gpio_select_alternate_function(GPIOK, GPIO_PIN_5, 6);

    // Use system clock
    PWM0->CC &= ~(PWM_CC_USEPWM);

    // Countdown mode, immediate updates to parameters
    PWM0->CTL = 0;
    PWM0->_0_GENA = 0x08C;
    PWM0->_0_GENB = 0x80C;

    // Set the period
    PWM0->_0_LOAD = 0x018F;

    // Pulse width for 25% duty cycle
    PWM0->_0_CMPA = 0x012B;

    // Pulse width for 75% duty cycle
    PWM0->_0_CMPB = 0x063;

    // Start the timer
    PWM0->CTL = 0x01;

    // Enable PWM
    PWM0->ENABLE = 0x03;
}
