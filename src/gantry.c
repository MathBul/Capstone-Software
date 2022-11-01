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

// Gantry commands
static gantry_command_t gantry_cmd_array[2];
static gantry_command_t* gantry_human_move_cmd = &gantry_cmd_array[0];
static gantry_command_t* gantry_robot_move_cmd = &gantry_cmd_array[1];

// Flag that gets set in the utils module when there is a system fault
extern bool utils_sys_fault;

// Previous and current chess boards
static chess_board_t previous_board;
static chess_board_t current_board;

// Flags
static bool robot_is_done = false;

/**
 * @brief Initializes all modules
 */
void gantry_init()
{
    // Prepare the gantry commands
    gantry_human_move_cmd->command.p_entry = &gantry_human_entry;
    gantry_human_move_cmd->command.p_action = &gantry_human_action;
    gantry_human_move_cmd->command.p_exit = &gantry_human_exit;
    gantry_human_move_cmd->command.p_is_done = &gantry_human_is_done;

    gantry_human_move_cmd->move.source_file = FILE_ERROR;
    gantry_human_move_cmd->move.source_rank = RANK_ERROR;
    gantry_human_move_cmd->move.dest_file = FILE_ERROR;
    gantry_human_move_cmd->move.dest_rank = RANK_ERROR;
    gantry_human_move_cmd->move.move_type = IDLE;

    gantry_robot_move_cmd->command.p_entry = &gantry_robot_entry;
    gantry_robot_move_cmd->command.p_action = &gantry_robot_action;
    gantry_robot_move_cmd->command.p_exit = &gantry_robot_exit;
    gantry_robot_move_cmd->command.p_is_done = &gantry_robot_is_done;
    
    gantry_robot_move_cmd->move.source_file = FILE_ERROR;
    gantry_robot_move_cmd->move.source_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.dest_file = FILE_ERROR;
    gantry_robot_move_cmd->move.dest_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.move_type = IDLE;
    
    // System level initialization of all other modules
    clock_sys_init();
    clock_timer0a_init(); // x
    clock_timer1a_init(); // y
    clock_timer2a_init(); // z
    clock_timer3a_init(); // switches
    clock_timer4a_init(); // gantry
    clock_timer5a_init(); // delay
    command_queue_init();
    stepper_init_motors();
    rpi_init();
}

// Adds itself to the command queue
void gantry_start()
{
    command_queue_push((command_t*)gantry_robot_move_cmd);
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
        //stepper_y_disable();
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
    gantry_command->move.source_file = FILE_ERROR;
    gantry_command->move.source_rank = RANK_ERROR;
    gantry_command->move.dest_file = FILE_ERROR;
    gantry_command->move.dest_rank = RANK_ERROR;
    gantry_command->move.move_type = IDLE;

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

    // Clear the data in the next gantry_move command
    gantry_clear_command(gantry_human_move_cmd);

    // Place the gantry_move command on the queue
    command_queue_push((command_t*)gantry_human_move_cmd);
    // Transmit the move to the RPi (9 bytes long)
    rpi_transmit(message, 9);
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
    // Reset everything
    robot_is_done = false;
    gantry_robot_move_cmd->move.source_file = FILE_ERROR;
    gantry_robot_move_cmd->move.source_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.dest_file = FILE_ERROR;
    gantry_robot_move_cmd->move.dest_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.move_type = IDLE;
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
    char move[5]; // NOTE: May not be used if ILLEGAL_MOVE is received
//    char start_instr_op_len[2];
//    uint8_t instr;
//    uint8_t op_len;

//    char checksum[2];
//
//    // First, read the first two bytes of the entire message
//    if (rpi_receive(start_instr_op_len, 2)) {
//        if (start_instr_op_len[0] == START_BYTE)
//        {
//            instr = start_instr_op_len[1] >> 4;
//            op_len = start_instr_op_len[1] & (~0xF0);
//        }
//        else
//        {
//            // Error occurred; no message should begin without 0x0A
//        }
//    }
    if (rpi_receive(move, 5))
    {
        /*
         * Dev notes:
         * Order of operations: (if timeout at any point reset)
         * 1. Wait for start byte
         * 2. Once start sequence is received, read the order and assign to variables
         * 3. Calculate checksum
         * 4. If checksum passes, send an ack and return the move struct
         * 4b. If checksum fails, send a bad ack and goto step 1.
         */
        p_gantry_command->move.source_file = utils_byte_to_file(move[0]);
        p_gantry_command->move.source_rank = utils_byte_to_rank(move[1]);
        p_gantry_command->move.dest_file = utils_byte_to_file(move[2]);
        p_gantry_command->move.dest_rank = utils_byte_to_rank(move[3]);
        p_gantry_command->move.move_type = utils_byte_to_move_type(move[4]);

        robot_is_done = true; // We've got the data we need
    }
// Keenan your stuff starts here
//    char check_bytes[2];
//    char message[7];
//
//    // First, read the first two bytes of the entire message
//    if (rpi_receive(start_instr_op_len, 2))
//    {
//        if (start_instr_op_len[0] == START_BYTE)
//        {
//            instr = start_instr_op_len[1] >> 4;
//            op_len = start_instr_op_len[1] & (~0xF0);
//        }
//        else
//        {
//            // Error occurred; no message should begin without 0x0A
//        }
//    }
//
//    // Depending on the instruction, take a particular action
//    if (instr == ROBOT_MOVE_INSTR)
//    {
//        if (rpi_receive(move, 5))
//        {
//            p_gantry_command->move.source_file = rpi_byte_to_file(move[0]);
//            p_gantry_command->move.source_rank = rpi_byte_to_rank(move[1]);
//            p_gantry_command->move.dest_file = rpi_byte_to_file(move[2]);
//            p_gantry_command->move.dest_rank = rpi_byte_to_rank(move[3]);
//            message[0] = start_instr_op_len[0];
//            message[1] = start_instr_op_len[1];
//            message[2] = move[0];
//            message[3] = move[1];
//            message[4] = move[2];
//            message[5] = move[3];
//            message[6] = move[4];
//            rpi_receive(check_bytes, 2);
//            if (!utils_validate_transmission(message, 7, check_bytes))
//            {
//                // Checksum error; corrupted data
//            }
//        }
//    }
//    else if (instr == ILLEGAL_MOVE_INSTR)
//    {
//        // Still player's turn; robot will not move.
//        message[0] = start_instr_op_len[0];
//        message[1] = start_instr_op_len[1];
//        rpi_receive(check_bytes, 2);
//        if (!utils_validate_transmission(message, 2, check_bytes))
//        {
//            // Checksum error; corrupted data
//        }
//    }
//    else
//    {
//        // Error occurred; found no matching instruction
//    }
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
    
    // We should have the necessary data from the action function

    // TODO: Fill this in to load the next commands. Very rough pseudocode follows!
    // lc = led_build_command()
    // command_queue_push(lc)
    
     switch (p_gantry_command->move.move_type) {
         case MOVE:
             // Error checking
             if (p_gantry_command->move.source_file == FILE_ERROR || p_gantry_command->move.source_rank == RANK_ERROR)
             {
                 break;
             }
             // Move to the piece to move
             // the enums are the absolute positions of those ranks/file. current_pos is also absolute
             command_queue_push
             (
                 (command_t*)stepper_build_chess_command
                 (
                     p_gantry_command->move.source_file,  // file
                     p_gantry_command->move.source_rank,  // rank
                     1,                                   // v_x
                     1,                                   // v_y
                     0                                    // v_z
                 )
             );
             // wait
             command_queue_push((command_t*)delay_build_command(1000));
             // lower the lifter
             // grab the piece
             // raise the lifter
             // move to the dest
             // the enums are the absolute positions of those ranks/file. current_pos is also absolute
             command_queue_push
             (
                 (command_t*)stepper_build_chess_command
                 (
                     p_gantry_command->move.dest_file, // file
                     p_gantry_command->move.dest_rank, // rank
                     1,                                // v_x
                     1,                                // v_y
                     0                                 // v_z
                 )
             );
             // wait
             command_queue_push((command_t*)delay_build_command(1000));
             // lower the lifter
             // release the piece
             // raise the lifter
             // home
         break;
         case PROMOTION:
             // TODO
         break;
        
         case CAPTURE:
             // TODO
         break;

         case CASTLING:
             // TODO
         break;

         case EN_PASSENT:
             // TODO
         break;

         default:
             // TODO
         break;
     }

    // lc = led_build_command()
    // command_queue_push(lc)
    // snc = sensor_network_build_command()
    // command_queue_push(snc)
     // Do it again !
     command_queue_push((command_t*)gantry_robot_move_cmd);
}

/**
 * @brief Moves to the next command once a move has been received from the RPi
 * 
 * @param command The gantry command being run
 * @return true If the RPi responded
 */
bool gantry_robot_is_done(command_t* command)
{
    return robot_is_done;
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
