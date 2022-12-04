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

// Function declarations
void pwm_init(void);


#endif /* PWM_H_ */
