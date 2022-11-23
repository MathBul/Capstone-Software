/**
 * @file gpio.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Functions to control general purpose input/output (GPIO) pins using register-level code
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#include "gpio.h"

/* GPIO Output Functions */

/**
 * @brief Sets the specified GPIO port/pin as an output, then clears the output value
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
void gpio_set_as_output(GPIO_Type* port, uint8_t pin)
{
    utils_gpio_clock_enable(port);
    port->DIR |= pin;
    port->DEN |= pin;
    port->DATA &= ~pin;
}

/**
 * @brief Sets the output of the specified GPIO port/pin as high
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
void gpio_set_output_high(GPIO_Type* port, uint8_t pin)
{
    port->DATA |= pin;
}

/**
 * @brief Sets the output of the specified GPIO port/pin as low
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
void gpio_set_output_low(GPIO_Type* port, uint8_t pin)
{
    port->DATA &= ~pin;
}

/**
 * @brief Toggles the output of the specified GPIO port/pin
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
void gpio_set_output_toggle(GPIO_Type* port, uint8_t pin)
{
    port->DATA ^= pin;
}

/* GPIO Input Functions */

/**
 * @brief Sets the specified GPIO port/pin as an input
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
void gpio_set_as_input(GPIO_Type* port, uint8_t pin)
{
    utils_gpio_clock_enable(port);
    port->DIR &= ~pin;
    port->DEN |= pin;
}

/**
 * @brief Reads the specified GPIO port/pin. Returns either a 0 or a 1
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 */
uint8_t gpio_read_input(GPIO_Type* port, uint8_t pin)
{
    // Produce the result
    return (port->DATA & pin);
}

/**
 * @brief Selects an alternate pin functionality. Modfies GPIO alternate function select and port control registers
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, GPIO_PIN_X for X={0,...,7}
 * @param multiplex_val The select for the alternate function multiplexer
 */
void gpio_select_alternate_function(GPIO_Type* port, uint8_t pin, uint8_t multiplex_val)
{
    port->AFSEL |= pin;
    uint8_t lsb_shift = utils_bits8_get_lsb_shift(pin);
    port->PCTL  |= (multiplex_val << (lsb_shift << 2));
}

/* End gpio.c */
