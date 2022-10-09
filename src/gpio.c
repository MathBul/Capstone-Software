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
#include "utils.h"

/* GPIO Output Functions */

/**
 * @brief Sets the specified GPIO port/pin as an output, then resets the output value
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, should be one of GPIO_PIN_X
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
 * @param pin Pin being used, should be one of GPIO_PIN_X
 */
void gpio_set_output_high(GPIO_Type* port, uint8_t pin)
{
    port->DATA |= pin;
}

/**
 * @brief Sets the output of the specified GPIO port/pin as low
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, should be one of GPIO_PIN_X
 */
void gpio_set_output_low(GPIO_Type* port, uint8_t pin)
{
    port->DATA &= ~pin;
}

/**
 * @brief Toggles the output of the specified GPIO port/pin
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, should be one of GPIO_PIN_X
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
 * @param pin Pin being used, should be one of GPIO_PIN_X
 */
void gpio_set_as_input(GPIO_Type* port, uint8_t pin)
{
    utils_gpio_clock_enable(port);
    port->DIR &= ~pin;
    port->DEN |= pin;
}

/**
 * @brief Selects the alternate pin functionality. Modfies the GPIO alternate select and port control 
 * registers
 * 
 * @param port GPIO_Type for the port being used
 * @param pin Pin being used, should be one of GPIO_PIN_X
 * @param multiplex_val The select to multiplexer
 */
void gpio_select_alternate(GPIO_Type* port, uint8_t pin, uint8_t multiplex_val)
{
    // TODO: assert at least 1 bit in pin is set
    uint8_t lcb = 0;
    uint8_t shift_pin = pin;
    port->AFSEL |= pin;

    // Does not handle case of no bits set
    for (lcb = 0; lcb < 8; lcb++)
    {
        if ((shift_pin & 1) != 1)
        {
            shift_pin = shift_pin >> 1;
        }
        else
        {
            break;
        }
    }
    port->PCTL |= (multiplex_val << (lcb << 2));
}


/* End gpio.c */
