/**
 * @file pwm.c
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for using PWM on the MSP432E401Y
 * @version 0.1
 * @date 2022-12-02
 *
 * @copyright Copyright (c) 2022
 */

#include "pwm.h"

/*
 * @brief Initializes PWM at 10 KHz on pins PK4 and PK5 at the duty cycles specified
 *
 * Additional Notes:
 *   This PWM uses a counter in down mode. Additionally, it is set up to drive
 *   the pin low at a particular comparator value, and high at a particular load
 *   value (the max value of the counter)
 *   PK4: M0PWM6, sources from pwm3A'
 *   PK5: M0PWM7, sources from pwm3B'
 *
 * @param duty1 Duty cycle for PK4, an integer [0..100]
 * @param duty2 Duty cycle for PK5, an integer [0..100]
 */
void pwm_init(uint8_t duty_pk4, uint8_t duty_pk5)
{
    // Enable the PWM0 clock
    SYSCTL->RCGCPWM |= SYSCTL_RCGCPWM_R0;

    // Wait for the clock to be ready
    while (!(SYSCTL->RCGCPWM & SYSCTL_RCGCPWM_R0))
    {
    }

    // Set the pins as outputs
    gpio_set_as_output(PWM_PORT, PWM_A_PIN);
    gpio_set_as_output(PWM_PORT, PWM_B_PIN);

    // Enable PWM as alternate function on PK4 and PK5
    gpio_select_alternate_function(PWM_PORT, PWM_A_PIN, 6);
    gpio_select_alternate_function(PWM_PORT, PWM_B_PIN, 6);

    // Use 120 MHz system clock, divide by 8 for 15 MHz
    PWM0->CC |= (PWM_CC_USEPWM);    // Use the divider
    PWM0->CC &= ~(0x7);             // Clear the bottom 3 bits
    PWM0->CC |= PWM_CC_PWMDIV_8;    // Set the correct clock div

    // Countdown mode, immediate updates to parameters
    PWM0->CTL = 0;
    PWM0->_3_GENA = (PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE);
    PWM0->_3_GENB = (PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE);

    // Set the period w/load value
    PWM0->_3_LOAD = 1499;

    // PK4: Pulse width for 30% duty cycle
    pwm_set_duty_pk4(duty_pk4);

    // PK5: Pulse width for 30% duty cycle
    pwm_set_duty_pk5(duty_pk5);

    // Start the timer
    PWM0->CTL = PWM_0_CTL_ENABLE;

    // Enable the 3rd generator block
    PWM0->_3_CTL |= PWM_3_CTL_ENABLE;

    // Enable PWM on PK4 and PK5 (M0PWM6 and M0PWM7, respectively)
    PWM0->ENABLE |= (PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN);
}

/*
 * @brief Set the PWM duty cycle on pin PK4
 *
 * @param duty The duty cycle, an integer [0..100]
 */
void pwm_set_duty_pk4(uint8_t duty)
{
    if (duty == 0)
    {
        PWM0->_3_CMPA = PWM_LOAD_VAL-1;
    }
    else
    {
        PWM0->_3_CMPA = (uint32_t) (PWM_LOAD_VAL*(100 - duty))/100;
    }
}

/*
 * @brief Set the PWM duty cycle on pin PK5
 *
 * @param duty The duty cycle, an integer [0..100]
 */
void pwm_set_duty_pk5(uint8_t duty)
{
    if (duty == 0)
    {
        PWM0->_3_CMPB = PWM_LOAD_VAL-1;
    }
    else
    {
        PWM0->_3_CMPB = (uint32_t) (PWM_LOAD_VAL*(100 - duty))/100;
    }
}
