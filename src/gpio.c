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

/**
 * @brief Selects the alternate pin functionality. Modfies the GPIO alternate select and port control 
 * registers
 * 
 * @param port The GPIO_Type for the port being used
 * @param pin Which pin. Should be one of GPIO_PIN_X
 * @param multiplex_val The select to multiplexer
 */
void gpio_select_alternate(GPIO_Type* port, uint8_t pin, uint8_t multiplex_val)
{
    // TODO: assert at least 1 bit in pin is set
    uint8_t lcb = 0;
    uint8_t shift_pin = pin;
    port->AFSEL |= pin;

    // Doesn't really handle if there are no bits set
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


// End gpio.c
