#include "gpio.h"
#include "utils.h"

/* GPIO Output Functions */

/*
 * Sets the specified GPIO port/pin as an output, then resets the output value
 */
void gpio_set_as_output(GPIO_Type* port, uint8_t pin)
{
    utils_gpio_clock_enable(port);
    port->DIR |= pin;
    port->DEN |= pin;
    port->DATA &= ~pin;
}

/*
 * Sets the output of the specified GPIO port/pin as high
 */
void gpio_set_output_high(GPIO_Type* port, uint8_t pin)
{
    port->DATA |= pin;
}

/*
 * Sets the output of the specified GPIO port/pin as low
 */
void gpio_set_output_low(GPIO_Type* port, uint8_t pin)
{
    port->DATA &= ~pin;
}

/*
 * Toggle the output of the specified GPIO port/pin
 */
void gpio_set_output_toggle(GPIO_Type* port, uint8_t pin)
{
    port->DATA ^= pin;
}

/* GPIO Input Functions */

/*
 * Sets the specified GPIO port/pin as an output, then resets the output value
 */
void gpio_set_as_input(GPIO_Type* port, uint8_t pin)
{
    port->DIR &= ~pin;
    port->DEN |= pin;
    utils_gpio_clock_enable(port);
}

// End gpio.c
