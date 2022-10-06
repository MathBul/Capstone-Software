// Project-specific source code
#include "steppermotors.h"
#include "clock.h"
#include "gpio.h"
#include "uart.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

#define STEPPER_DEBUG
//#define UART_DEBUG


int main(void)
{
#ifdef UART_DEBUG
    char message[64];
    int i = 0;

    sysclock_initialize();
    uart_init(UART_CHANNEL_0);

    // Read whatever comes in to the message string.
    while (1)
    {
        // This is "blocking"
        uart_read_byte(UART_CHANNEL_0, (uint8_t*)&message[i]);
        i++;
        if (i > 63)
        {
            i = 0;
        }
    }
#endif
#ifdef STEPPER_DEBUG
    // Initialize the system clock
    sysclock_initialize();
    timer_0a_initialize();

    // Initialize the stepper motor(s)
    stepper_initialize_motors();

    // Go 250mm right, 250mm forward
    stepper_go_to_position(250, 250);

    while(1)
    {
    }
#endif
}

// End main.c
