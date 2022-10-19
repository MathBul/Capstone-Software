// Project-specific source code
#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "utils.h"
#include "gantry.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

//#define STEPPER_DEBUG
//#define UART_DEBUG
//#define CHESS_ROBOT_MAIN

int main(void)
{
    #ifdef CHESS_ROBOT_MAIN

    // Initialize the system clock to 120 MHz
    clock_sys_init();

    // Initialize UART3
    uart_init(UART_CHANNEL_3);

    // Initialize both boards to starting positions
    chess_board_init(&previous_board);
    chess_board_init(&current_board);

    // Counter variable
    int i = 0;

    // Variables for holding various segments of UART instructions
    char first_byte;
    char instr_and_op_len;
    char move[5];

//    rpi_transmit_start('W');
//    rpi_transmit_HUMAN_MOVE(move);
//    bool first_byte_received = rpi_receive(first_byte);
    while (1)
    {
        // Spin until the first byte is received
        bool first_byte_received = rpi_receive(&first_byte);
        if (first_byte == START_BYTE)
        {
            /* A byte holding the instruction and operand length (4 bits each)
               follows the start byte */
            bool second_byte_received = rpi_receive(&instr_and_op_len);

            // Extract the instruction bits
            uint8_t instr = instr_and_op_len >> 4;

            // Extract the operand length bits
            uint8_t op_len = instr_and_op_len & (~0xF0);

            // Using the instruction, determine what action to take
            if (instr == ROBOT_MOVE_INSTR)
            {
                bool move0_rec = rpi_receive(&move[0]);
                bool move1_rec = rpi_receive(&move[1]);
                bool move2_rec = rpi_receive(&move[2]);
                bool move3_rec = rpi_receive(&move[3]);
                bool move4_rec = rpi_receive(&move[4]);
                i++;
            }
            else if (instr == ILLEGAL_MOVE_INSTR)
            {
                i++;
            }
        }
        // Temporarily here as a breakpoint
        i++;
        if (i > 31)
        {
            i = 0;
        }
        // Reset first_byte and instr_and_op_len bytes for next instruction
        first_byte = 0xFF;
        instr_and_op_len = 0xFF;
    }
    #endif

    #ifdef UART_DEBUG
    char message[64];
    int i = 0;

    clock_sys_init();
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
    clock_sys_init();
    clock_timer0a_init();

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
