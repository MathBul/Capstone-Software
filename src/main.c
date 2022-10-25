// Project-specific source code
#include "gantry.h"

// Standard includes necessary for base functionality
#include "msp.h"
#include <stdint.h>

//#define STEPPER_DEBUG
//#define UART_DEBUG
//#define FUNCTION_DEBUG
//#define CHESS_ROBOT_MAIN
#define COMMAND_QUEUE

int main(void)
{
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

#ifdef COMMAND_QUEUE
    // Probably need to allocate enough memory for the largest command struct
    command_t current_command;

    // System level initialization
    clock_sys_init();
    clock_timer0a_init();
    command_queue_init();
    stepper_init_motors();
    uart_init(UART_CHANNEL_3);

    // Add commands to the queue

    // Main program flow
    while (1)
    {
        // Run the entry function
        if (!command_queue_pop(&current_command))
        {
            // Something went wrong. Probably ran out of commands
        }
        else
        {
            current_command.p_entry(&current_command);
        }
        // Run the action function. is_done determines when action is complete
        while (!current_command.p_is_done(&current_command))
        {
            // Check for a system fault (e-stop etc)
            if (false)
            {
                break;
            }
            current_command.p_action(&current_command);
        }
        // Run the exit function
        current_command.p_exit(&current_command);
    }
#endif

    #ifdef CHESS_ROBOT_MAIN

    // Initialize the system clock to 120 MHz
    clock_sys_init();

    // Initialize UART3
    uart_init(UART_CHANNEL_3);

    // Initialize boards
    chess_board_t previous_board;
    chess_board_t current_board;

    // Initialize both boards to starting positions
    chessboard_init(&previous_board);
    chessboard_init(&current_board);

    // Counter variable
    int i = 0;

    // Variables for holding various segments of UART instructions
    char first_byte;
    char instr_and_op_len;
    char move[5];

//    rpi_transmit_start('W');
    rpi_transmit_human_move("e2e4");
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
                // 1. Move the electromagnet above the start square
                // 2. Drop the electromagnet to pick up the piece
                //    a. This means checking the current board's board pieces array and adjusting
                //       drop distance based on the square being hovered
                // 3. Turn on the electromagnet
                // 4. Lift the electromagnet
                // 5. Move to the end square
                // 6. Drop the electromagnet, again distance based on the piece
                // 7. Turn off the electromagnet
                // 8. Return home
                i++;
            }
            else if (instr == ILLEGAL_MOVE_INSTR)
            {
                // 1. Turn on an LED to inform the user their last move was illegal
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

    #ifdef FUNCTION_DEBUG
    clock_sys_init();
    char squares[64][2];
    uint8_t indices[8];

    chess_board_t prev;
    chess_board_t fin;
    chessboard_init(&prev);
    chessboard_init(&fin);
    int i;
    char move[5];

    for (i = 0; i < 64; i++)
    {
        chessboard_index_to_square(i, squares[i]);
    }

    indices[0] = chessboard_square_to_index('e', '2');
    indices[1] = chessboard_square_to_index('e', '4');
    indices[2] = chessboard_square_to_index('c', '3');
    indices[3] = chessboard_square_to_index('d', '4');
    indices[4] = chessboard_square_to_index('e', '5');
    indices[5] = chessboard_square_to_index('f', '6');
    indices[6] = chessboard_square_to_index('g', '7');
    indices[7] = chessboard_square_to_index('h', '8');

    // after e2e4
//    fin.board_presence = (uint64_t) 0b1111111111111111000000000000000000010000000000001110111111111111;

    // after b1c3
//    fin.board_presence = (uint64_t) 0b1111111111111111000000000000000000000000000001001111111111111101;

    // after g7g5
    fin.board_presence = (uint64_t) 0b1111111110111111000000000100000000000000000000001111111111111111;

    chessboard_get_move(&prev, &fin, move);

    while (1) {}

    #endif
}

/* End main.c */
