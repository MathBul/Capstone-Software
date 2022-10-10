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
#include <stdint.h>

// GPIO pin definitions
#define GPIO_PIN_0                          ((uint8_t) (1 << 0))
#define GPIO_PIN_1                          ((uint8_t) (1 << 1))
#define GPIO_PIN_2                          ((uint8_t) (1 << 2))
#define GPIO_PIN_3                          ((uint8_t) (1 << 3))
#define GPIO_PIN_4                          ((uint8_t) (1 << 4))
#define GPIO_PIN_5                          ((uint8_t) (1 << 5))
#define GPIO_PIN_6                          ((uint8_t) (1 << 6))
#define GPIO_PIN_7                          ((uint8_t) (1 << 7))

// Functions for GPIO output
void gpio_set_as_output(GPIO_Type* port, uint8_t pin);
void gpio_set_output_high(GPIO_Type* port, uint8_t pin);
void gpio_set_output_low(GPIO_Type* port, uint8_t pin);
void gpio_set_output_toggle(GPIO_Type* port, uint8_t pin);

// Functions for GPIO input
void gpio_set_as_input(GPIO_Type* port, uint8_t pin);
uint8_t gpio_read_input(GPIO_Type* port, uint8_t pin);
void gpio_select_alternate(GPIO_Type* port, uint8_t pin, uint8_t multiplex_val);

#endif /* GPIO_H_ */
