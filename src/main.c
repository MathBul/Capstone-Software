// Project-specific source code
#include "steppermotors.h"
#include "clock.h"
#include "gpio.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>


int main(void)
{
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
}

// End main.c
