#ifndef GPIO_H_
#define GPIO_H_

#include "msp.h"

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

#endif /* GPIO_H_ */
