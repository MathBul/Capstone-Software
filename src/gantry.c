/**
 * @file gantry.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Code to unite all other modules. All ISRs are implemented here
 * @version 0.1
 * @date 2022-10-19
 * 
 * @copyright Copyright (c) 2022
 */

#include "gantry.h"

// Private functions
void gantry_limit_stop(uint8_t limit_readings);
void gantry_home();
void gantry_reset();
void gantry_kill();
void gantry_clear_command(gantry_command_t* gantry_command);

// Array of gantry commands
gantry_command_t gantry_cmds[COMMAND_QUEUE_SIZE];
static gantry_command_t* gantry_human_cmd = &gantry_cmds[0];
static gantry_command_t* gantry_robot_cmd = &gantry_cmds[1];

// Flag that gets set in the utils module when there is a system fault
extern bool utils_sys_fault;

// Previous and current chess boards
static chess_board_t previous_board;
static chess_board_t current_board;

/**
 * @brief Initializes all modules
 */
void gantry_init()
{
    // Prepare the gantry commands
    gantry_human_cmd->command.p_entry = &gantry_human_entry;
    gantry_human_cmd->command.p_action = &gantry_human_action;
    gantry_human_cmd->command.p_exit = &gantry_human_exit;
    gantry_human_cmd->command.p_is_done = &gantry_human_is_done;
    
    gantry_robot_cmd->command.p_entry = &gantry_robot_entry;
    gantry_robot_cmd->command.p_action = &gantry_robot_action;
    gantry_robot_cmd->command.p_exit = &gantry_robot_exit;
    gantry_robot_cmd->command.p_is_done = &gantry_robot_is_done;
    
    // System level initialization of all other modules
    clock_sys_init();
    clock_timer0a_init();
    clock_timer1a_init();
    clock_timer2a_init();
    clock_timer3a_init();
    clock_timer4a_init();
    clock_timer5a_init();
    command_queue_init();
    stepper_init_motors();
}

/**
 * @brief Interprets an instruction from the RPi, stored in the gantry_command struct
 * 
 * @param gantry_command The command containing the instruction
 */
void gantry_interpret(gantry_command_t* gantry_command)
{
    // TODO: Switch statement to interpret the instruction read from the Pi
    //  Store values in the gantry command struct for access in the gantry_exit() function
}

/**
 * @brief Stops steper motors based on the current limit switch readings
 * 
 * @param limit_readings A limit switch reading configured according to the switch vport
 */
void gantry_limit_stop(uint8_t limit_readings)
{
    if (limit_readings & LIMIT_X)
    {
        stepper_x_disable();
    }
    if (limit_readings & LIMIT_Y)
    {
        stepper_y_disable();
    }
    if (limit_readings & LIMIT_Z)
    {
        stepper_z_disable();
    }
}

/**
 * @brief Homes the gantry system (motors all the way up, right, back from pov of robot)
 */
void gantry_home()
{
    // Pause the motors
    stepper_pause_motors();

    // Clear the command queue
    command_queue_clear();

    // Add a homing command to the queue
    // TODO: Add stepper_home command

    // Resume motor motion
    stepper_pause_motors();
}

/**
 * @brief Resets the entire system (motors, stored chess boards, UART, etc.)
 */
void gantry_reset()
{
    // Clear the command queue
    command_queue_clear();

    // Home the motors
    gantry_home();

    // Reset the chess board
    chessboard_reset();

    // Reset the rpi
    rpi_reset_uart();

    // Start the new game
    // TODO: Use (switch_vport.image & ROCKER_COLOR) instead of hard-coding
    rpi_transmit_start('W');
}

/**
 * @brief Hard stops the gantry system. Kills (but does not home) motors, sets sys_fault flag
 */
void gantry_kill()
{
    // Disable all motors
    stepper_disable_all_motors();

    // Set the system fault flag
    utils_sys_fault = true;

    // Clear the command queue (just in case)
    command_queue_clear();
}

/**
 * @brief Clears a gantry command before it is placed back on the queue
 * 
 */
void gantry_clear_command(gantry_command_t* gantry_command)
{
    gantry_command->move[0]        = 0;
    gantry_command->move[1]        = 0;
    gantry_command->move[2]        = 0;
    gantry_command->move[3]        = 0;
    gantry_command->move[4]        = 0;
    gantry_command->move_type      = 0;
}

/* Command Functions */

/**
 * @brief Prepares the gantry for a read command. Nothing is done in this case
 * 
 * @param command The gantry command being run
 */
void gantry_human_entry(command_t* command)
{
    return;
}

/**
 * @brief Waits for user interaction
 * 
 * @param command The gantry command being run
 */
void gantry_human_action(command_t* command)
{
    return;
}

/**
 * @brief Interprets the RPi's move, and adds the appropriate commands to the queue
 * 
 * @param command The gantry command being run
 */
void gantry_human_exit(command_t* command)
{
    // Read the current board state
    sensors_read_tile(row_1, col_a);    // TODO: Read more than one tile (poll the board)

    // Set up char arrays
    char move[5];
    char check_bytes[2];
    char message[9];

    // Interpret the board state
    chessboard_get_move(&previous_board, &current_board, move);

    // This will always be a HUMAN_MOVE instruction
    message[0] = START_BYTE;
    message[1] = HUMAN_MOVE_INSTR_AND_LEN;
    message[2] = move[0];
    message[3] = move[1];
    message[4] = move[2];
    message[5] = move[3];
    message[6] = move[4];
    utils_fl16_data_to_cbytes((uint8_t *) message, 7, check_bytes);
    message[7] = check_bytes[0];
    message[8] = check_bytes[1];

    // Transmit the move to the RPi (9 bytes long)
    rpi_transmit(message, 9);

    // Clear the data in the next gantry_robot command
    gantry_clear_command(gantry_robot_cmd);

    // Place the gantry_robot command on the queue
    command_queue_push((command_t*)gantry_robot_cmd);
}

/**
 * @brief Moves to the next command once the END_TURN button has been pressed
 * 
 * @param command The gantry command being run
 * @return true If the END_TURN button has been pressed
 */
bool gantry_human_is_done(command_t* command)
{
    uint8_t switch_data = switch_vport.image;
    return (switch_data & BUTTON_END_TURN);
}

/**
 * @brief Prepares the gantry for a move command. Nothing is done in this case
 * 
 * @param command The gantry command being run
 */
void gantry_robot_entry(command_t* command)
{
    return;
}

/**
 * @brief Attempts to read from the RPi until data has been received
 * 
 * @param command The gantry command being run
 */
void gantry_robot_action(command_t* command)
{
    gantry_command_t* p_gantry_command = (gantry_command_t*) command;
    // TODO: Waits, checksums, etc. as needed

    // Receive a move from the RPi and load it into the command struct for the exit() function

    /* RPi can send back 3 instructions: ROBOT_MOVE, ILLEGAL_MOVE,
     * or GAME_STATUS. Only the first two should be possible here.
     * GAME_STATUS will only be sent in response to a legal move. */

    /* Receive arrays */
    char start_instr_op_len[2];
    uint8_t instr;
    uint8_t op_len;
    char move[5]; // NOTE: May not be used if ILLEGAL_MOVE is received
    char checksum[2];

    // First, read the first two bytes of the entire message
    if (rpi_receive(start_instr_op_len, 2)) {
        if (start_instr_op_len[0] == START_BYTE)
        {
            instr = start_instr_op_len[1] >> 4;
            op_len = start_instr_op_len[1] & (~0xF0);
        }
        else
        {
            // Error occurred; no message should begin without 0x0A
        }
    }
    if (rpi_receive(move, op_len))
    {
        p_gantry_command->move[0] = move[0];
        p_gantry_command->move[1] = move[1];
        p_gantry_command->move[2] = move[2];
        p_gantry_command->move[3] = move[3];
        p_gantry_command->move[4] = move[4];
    }
    // Validate the check bytes
}

/**
 * @brief Interprets the RPi's move, and adds the appropriate commands to the queue
 *        Additionally, waits for the game status from RPi.
 *
 * @param command The gantry command being run
 */
void gantry_robot_exit(command_t* command)
{
    gantry_command_t* p_gantry_command = (gantry_command_t*) command;
    
    // Interpret the move
    gantry_interpret(p_gantry_command);

    // TODO: Fill this in to load the next commands. Very rough pseudocode follows!
    // lc = led_build_command()
    // command_queue_push(lc)
    
    // switch (p_gantry_command->move_type) {
    //     case 'M':   // Move
    //         sc = stepper_build_command()
    //         command_queue_push(sc)
    //         ec = electromagnet_build_command()
    //         command_queue_push(ec)
    //         sc = stepper_build_command()
    //         command_queue_push(sc)
    //         ec = electromagnet_build_command()
    //         command_queue_push(ec)
    //         sc = stepper_build_command()
    //         command_queue_push(sc)
    //         break;  
    //     case 'P':   // Promotion
    //         // TODO
    //         break;
        
    //     case 'C':   // Capture
    //         // TODO 
    //         break;

    //     case 'c':   // Castling
    //         // TODO
    //         break;

    //     case 'E':   // En passante
    //         // TODO
    //         break;

    //     default:
    //         // TODO
    //         break;
    // }

    // lc = led_build_command()
    // command_queue_push(lc)
    // snc = sensor_network_build_command()
    // command_queue_push(snc)
}

/**
 * @brief Moves to the next command once a move has been received from the RPi
 * 
 * @param command The gantry command being run
 * @return true If the RPi responded
 */
bool gantry_robot_is_done(command_t* command)
{
    return true;
    // TODO: Some sort of check that RPi.receive() found data
}

/**
 * @brief Interrupt handler for the gantry module
 */
__interrupt void GANTRY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(GANTRY_TIMER);
    
    // Check the current switch readings
    uint8_t switch_data = switch_vport.image;

    // If the emergency stop button was pressed, kill everything
    if (switch_data & BUTTON_ESTOP)
    {
        gantry_kill();
    }

    // If a limit switch was pressed, disable the appropriate motor
    uint8_t limit_reading = ((switch_data) & (LIMIT_X | LIMIT_Y | LIMIT_Z));
    gantry_limit_stop(limit_reading);

    // If the start/reset button was pressed, send the appropriate "new game" signal
    if (switch_data & BUTTON_START_RESET)
    {
        gantry_reset();
    }

    // If the home button was pressed, clear the queue and execute a homing command
    if (switch_data & BUTTON_HOME)
    {
        gantry_home();
    }
}

/* End gantry.c */
