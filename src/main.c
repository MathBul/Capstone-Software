// Project-specific source code
#include "clock.h"
#include "gpio.h"
#include "steppermotors.h"
#include "uart.h"
#include "testing.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

int main(void)
{
    #ifdef CHESS_ROBOT_MAIN
    sysclock_init();
    uart_init(UART_CHANNEL_3);

//    int i = 0;
    char start_signal = 'A';
    char player_color = 'B';
//    char move_data[8];

    uart_out_byte(UART_CHANNEL_3, (uint8_t) 'S');
    while (start_signal != 'S')
    {
        uart_read_byte(UART_CHANNEL_3, (uint8_t*)&start_signal);
    }
    if (player_color == 'W')
        uart_out_byte(UART_CHANNEL_3, (uint8_t) 'W');
    else {
        uart_out_byte(UART_CHANNEL_3, (uint8_t) 'B');
    }

    while (1) {}

    #endif

    #ifdef UART_DEBUG
    char message[64];
    int i = 0;

    sysclock_init();
    uart_init(UART_CHANNEL_3);

    // Read whatever comes in to the message string.
    while (1)
    {
        // This is "blocking"
        uart_read_byte(UART_CHANNEL_3, (uint8_t*)&message[i]);
//        uart_out_byte(UART_CHANNEL_3, (uint8_t) 'A');
        i++;
        if (i > 63)
        {
            i = 0;
        }
    }
    #endif

    #ifdef STEPPER_DEBUG
    // Initialize the system clock and timer(s)
    sysclock_init();
    timer_0a_init();

    // Initialize the stepper motor(s)
    stepper_init_motors();

    // Serious of position commands
    stepper_go_home();
    stepper_go_to_rel_position(250, 0);
    stepper_go_to_rel_position(0, 250);
    stepper_go_home();
    stepper_go_to_rel_position(200, 200);
    stepper_go_to_rel_position(50, -50);
    stepper_go_to_rel_position(-50, 50);
    stepper_go_home();
    stepper_go_to_rel_position(250, 250);
    int i = 0;
    for (i = 3; i < 8; i++)
    {
        stepper_go_to_rel_position(0, 10*i);
        stepper_go_to_rel_position(10*i, 0);
        stepper_go_to_rel_position(-20*i, -20*i);
        stepper_go_to_rel_position(10*i, 0);
        stepper_go_to_rel_position(0, 10*i);
    }
    stepper_go_home();

    while(1)
    {
    }
    #endif
}

/* End main.c */
