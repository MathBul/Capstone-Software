/**
 * @file utils.h
 * @author Nick Cooney (npc4crc@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Utility functions that may be used by more than one module
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#include "utils.h"

/**
 * @brief Enable the specified GPIO port by assigning it a clock
 * 
 * @param port GPIO_Type for the port being used
 */
void utils_gpio_clock_enable(GPIO_Type* port)
{
    uint32_t clock_gate_control_bit = 0;

    // Set the clock gate control bit based on the port
    if (port == GPIOA)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R0;
    }
    else if (port == GPIOB)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R1;
    }
    else if (port == GPIOC)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R2;
    }
    else if (port == GPIOD)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R3;
    }
    else if (port == GPIOE)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R4;
    }
    else if (port == GPIOF)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R5;
    }
    else if (port == GPIOG)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R6;
    }
    else if (port == GPIOH)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R7;
    }
    else if (port == GPIOJ)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R8;
    }
    else if (port == GPIOK)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R9;
    }
    else if (port == GPIOL)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R10;
    }
    else if (port == GPIOM)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R11;
    }
    else if (port == GPION)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R12;
    }
    else if (port == GPIOP)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R13;
    }
    else if (port == GPIOQ)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R14;
    }

    // Enable clock gate control
    SYSCTL->RCGCGPIO |= clock_gate_control_bit;

    // Wait for the port clock to be ready
    while (!(SYSCTL->PRGPIO & clock_gate_control_bit))
    {
    }
}

/**
 * @brief Delay the next operation with a for-loop (used for spacing out UART communication)
 *
 * @param ticks the number of clock ticks to delay for
 */
void utils_delay(uint32_t ticks)
{
    int i;
    for (i = 0; i < ticks; i++) {}
}

/* End utils.c */
