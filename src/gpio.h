/**
 * @file gpio.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Functions to control general purpose input/output (GPIO) pins using register-level code
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "msp.h"
#include "utils.h"
#include <stdint.h>

// GPIO pin definitions
#define GPIO_PIN_0                          (BITS8_MASK(0))
#define GPIO_PIN_1                          (BITS8_MASK(1))
#define GPIO_PIN_2                          (BITS8_MASK(2))
#define GPIO_PIN_3                          (BITS8_MASK(3))
#define GPIO_PIN_4                          (BITS8_MASK(4))
#define GPIO_PIN_5                          (BITS8_MASK(5))
#define GPIO_PIN_6                          (BITS8_MASK(6))
#define GPIO_PIN_7                          (BITS8_MASK(7))

// Functions for GPIO output
void gpio_set_as_output(GPIO_Type* port, uint8_t pin);
void gpio_unlock(GPIO_Type* port, uint8_t pin);
void gpio_set_output_high(GPIO_Type* port, uint8_t pin);
void gpio_set_output_low(GPIO_Type* port, uint8_t pin);
void gpio_set_output_toggle(GPIO_Type* port, uint8_t pin);

// Functions for GPIO input
void gpio_set_as_input(GPIO_Type* port, uint8_t pin);
uint8_t gpio_read_input(GPIO_Type* port, uint8_t pin);
void gpio_select_alternate_function(GPIO_Type* port, uint8_t pin, uint8_t multiplex_val);

#endif /* GPIO_H_ */
