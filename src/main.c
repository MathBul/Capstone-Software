// Project-specific source code
#include "clock.h"
#include "gpio.h"
#include "steppermotors.h"
#include "uart.h"
#include "raspberrypi.h"
#include "utils.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

//#define STEPPER_DEBUG
//#define UART_DEBUG
#define CHESS_ROBOT_MAIN

int main(void)
{
    #ifdef CHESS_ROBOT_MAIN
    sysclock_init();
    uart_init(UART_CHANNEL_3);
    long prev_board = 0xFF0000FF;
    long curr_board = 0xFF0000FF;
    int i = 0;
    int j = 0;
    char first_byte;
    char instr_and_op_len;
    char move[5];
//    rpi_transmit_START_W();
//    rpi_transmit_HUMAN_MOVE(move);
//    bool first_byte_received = rpi_receive(first_byte);
    while (1)
    {
        bool first_byte_received = rpi_receive(&first_byte);
        if (first_byte == START_BYTE)
        {
            bool second_byte_received = rpi_receive(&instr_and_op_len);

            uint8_t instr = instr_and_op_len >> 4;

            uint8_t op_len = instr_and_op_len & (~0xF0);
            if (instr == ROBOT_MOVE_INSTR)
            {
                bool move0_rec = rpi_receive(&move[0]);
                bool move1_rec = rpi_receive(&move[1]);
                bool move2_rec = rpi_receive(&move[2]);
                bool move3_rec = rpi_receive(&move[3]);
                bool move4_rec = rpi_receive(&move[4]);
                j++;
            }
            else if (instr == ILLEGAL_MOVE_INSTR)
            {
                j++;
            }
        }
        i++;
        if (i > 31)
        {
            i = 0;
        }
        first_byte = 0xFF;
        instr_and_op_len = 0xFF;
    }
//    char start_signal = 'A';
//    char player_color = 'B';

//    uart_out_byte(UART_CHANNEL_3, (uint8_t) 'S');
//    while (start_signal != 'S')
//    {
//        uart_read_byte(UART_CHANNEL_3, (uint8_t*)&start_signal);
//    }
//    if (player_color == 'W')
//        uart_out_byte(UART_CHANNEL_3, (uint8_t) 'W');
//    else {
//        uart_out_byte(UART_CHANNEL_3, (uint8_t) 'B');
//    }
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
