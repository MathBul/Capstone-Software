// Project-specific source code
#include "steppermotors.h"
#include "clock.h"
#include "gpio.h"
#include "uart.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

//#define STEPPER_DEBUG
#define UART_DEBUG


int main(void)
{
#ifdef UART_DEBUG
    sysclock_initialize();
    uart_init(UART_CHANNEL_0);

    while (1)
    {
        uart_out_byte(UART_CHANNEL_0, 'h');
    }
#endif
#ifdef STEPPER_DEBUG
    // Initialize the system clock
    sysclock_initialize();
    timer_0a_initialize();

    // Initialize the stepper motor(s)
    stepper_initialize_motors();

    // Go 250 mm in both x and y directions
    stepper_go_to_position(250, 250);

    while(1)
    {
    }
#endif
}

// End main.c
