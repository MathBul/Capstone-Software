#include "utils.h"

/*
 * Enable the specified GPIO port by assigning it a clock
 */
void utils_gpio_clock_enable(GPIO_Type* port)
{
    uint32_t clock_gating_control_bit = 0;

    if (port == GPIOA)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R0;
    }
    else if (port == GPIOB)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R1;
    }
    else if (port == GPIOC)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R2;
    }
    else if (port == GPIOD)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R3;
    }
    else if (port == GPIOE)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R4;
    }
    else if (port == GPIOF)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R5;
    }
    else if (port == GPIOG)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R6;
    }
    else if (port == GPIOH)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R7;
    }
    else if (port == GPIOJ)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R8;
    }
    else if (port == GPIOK)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R9;
    }
    else if (port == GPIOL)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R10;
    }
    else if (port == GPIOM)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R11;
    }
    else if (port == GPION)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R12;
    }
    else if (port == GPIOP)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R13;
    }
    else if (port == GPIOQ)
    {
        clock_gating_control_bit = SYSCTL_RCGCGPIO_R14;
    }
    SYSCTL->RCGCGPIO |= clock_gating_control_bit;

    // Wait for the port clock to be ready
    while (!(SYSCTL->PRGPIO & clock_gating_control_bit))
    {
    }
}
