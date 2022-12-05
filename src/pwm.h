/**
 * @file pwm.h
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Code for using PWM on the MSP432E401Y
 * @version 0.1
 * @date 2022-12-02
 *
 * @copyright Copyright (c) 2022
 */

#ifndef PWM_H_
#define PWM_H_

// Uses pins K4 and K5 for IN1 and IN2, respectively
#include "msp.h"
#include "gpio.h"

#define PWM_CLOCK_FREQ   15000000
#define PWM_FREQ         10000
#define PWM_LOAD_VAL     1499

// Function declarations
void pwm_init(uint8_t duty_pk4, uint8_t duty_pk5);
void pwm_set_duty_pk4(uint8_t duty);
void pwm_set_duty_pk5(uint8_t duty);


#endif /* PWM_H_ */
