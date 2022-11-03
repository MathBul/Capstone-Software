/**
 * @file gantry.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Code to unite all other modules
 * @version 0.1
 * @date 2022-10-19
 * 
 * @copyright Copyright (c) 2022
 */

#include "gantry.h"

//#define KEENAN_TEST

// Homing flag
static bool gantry_homing = false;

// Private functions
void gantry_limit_stop(uint8_t limit_readings);
void gantry_home();
void gantry_reset();
void gantry_kill();
void gantry_clear_command(gantry_command_t* gantry_command);

// Previous and current chess boards
static chess_board_t previous_board;
static chess_board_t current_board;

// Flags
static bool robot_is_done = false;
extern bool utils_sys_fault;

/**
 * @brief Initializes all modules
 */
void gantry_init()
{
    // System level initialization of all other modules
    clock_sys_init();
    clock_timer0a_init();               // X
    clock_timer1a_init();               // Y
    clock_timer2a_init();               // Z
    clock_timer3a_init();               // Switches
    clock_timer4a_init();               // Gantry
    clock_timer5a_init();               // Delay
    clock_start_timer(SWITCH_TIMER);
    clock_start_timer(GANTRY_TIMER);
    command_queue_init();
    switch_init();
    stepper_init_motors();
    rpi_init();
}

// Adds itself to the command queue
void gantry_start()
{
    gantry_home();
    command_queue_push((command_t*)gantry_robot_build_command());
}

/**
 * @brief Stops stepper motors based on the current limit switch readings
 *  TODO: Change functionality beyond kill
 * 
 * @param limit_readings A limit switch reading configured according to the switch vport
 */
void gantry_limit_stop(uint8_t limit_readings)
{
    if ((!gantry_homing) && (limit_readings & (LIMIT_X | LIMIT_Y | LIMIT_Z)))
    {
        gantry_kill();
    }
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
    stepper_x_stop();
    stepper_y_stop();
    stepper_z_stop();

    // Set the system fault flag
    utils_sys_fault = true;

    // Clear the command queue (just in case)
    command_queue_clear();
}

/* Command Functions */

/**
 * @brief Build a gantry_human command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_human_build_command()
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*) malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &utils_empty_function;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &gantry_human_exit;
    p_command->command.p_is_done = &gantry_human_is_done;

    // Data
    p_command->move.source_file = FILE_ERROR;
    p_command->move.source_rank = RANK_ERROR;
    p_command->move.dest_file   = FILE_ERROR;
    p_command->move.dest_rank   = RANK_ERROR;
    p_command->move.move_type   = IDLE;

    return p_command;
}

/**
 * @brief Interprets the RPi's move, and adds the appropriate commands to the queue
 *  TODO: Incomplete
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

    // Place the gantry_move command on the queue
    command_queue_push((command_t*)gantry_human_build_command());

    // Transmit the move to the RPi (9 bytes long)
    rpi_transmit(message, 9);
}

/**
 * @brief Moves to the next command once the END_TURN button has been pressed
 *  TODO: Incomplete
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
 * @brief Build a gantry_robot command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_robot_build_command()
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*)malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_robot_entry;
    p_command->command.p_action  = &gantry_robot_action;
    p_command->command.p_exit    = &gantry_robot_exit;
    p_command->command.p_is_done = &gantry_robot_is_done;

    // Data
    p_command->move.source_file = FILE_ERROR;
    p_command->move.source_rank = RANK_ERROR;
    p_command->move.dest_file   = FILE_ERROR;
    p_command->move.dest_rank   = RANK_ERROR;
    p_command->move.move_type   = IDLE;

    return p_command;
}

/**
 * @brief Prepares the gantry for a move command. Nothing is done in this case
 * 
 * @param command The gantry command being run
 */
void gantry_robot_entry(command_t* command)
{
    gantry_command_t* gantry_robot_move_cmd = (gantry_command_t*) command;
    // Reset everything
    robot_is_done = false;
    gantry_robot_move_cmd->move.source_file = FILE_ERROR;
    gantry_robot_move_cmd->move.source_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.dest_file   = FILE_ERROR;
    gantry_robot_move_cmd->move.dest_rank   = RANK_ERROR;
    gantry_robot_move_cmd->move.move_type   = IDLE;
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
        p_gantry_command->move.dest_file   = utils_byte_to_file(move[2]);
        p_gantry_command->move.dest_rank   = utils_byte_to_rank(move[3]);
        p_gantry_command->move.move_type   = utils_byte_to_move_type(move[4]);

        robot_is_done = true; // We've got the data we need
    }

#ifdef KEENAN_TEST
    char start_instr_op_len[2];
    uint8_t instr;
    uint8_t op_len;

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

    char check_bytes[2];
    char message[7];

    // First, read the first two bytes of the entire message
    if (rpi_receive(start_instr_op_len, 2))
    {
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

    // Depending on the instruction, take a particular action
    if (instr == ROBOT_MOVE_INSTR)
    {
        if (rpi_receive(move, 5))
        {
            p_gantry_command->move.source_file = rpi_byte_to_file(move[0]);
            p_gantry_command->move.source_rank = rpi_byte_to_rank(move[1]);
            p_gantry_command->move.dest_file = rpi_byte_to_file(move[2]);
            p_gantry_command->move.dest_rank = rpi_byte_to_rank(move[3]);
            message[0] = start_instr_op_len[0];
            message[1] = start_instr_op_len[1];
            message[2] = move[0];
            message[3] = move[1];
            message[4] = move[2];
            message[5] = move[3];
            message[6] = move[4];
            rpi_receive(check_bytes, 2);
            if (!utils_validate_transmission(message, 7, check_bytes))
            {
                // Checksum error; corrupted data
            }
        }
    }
    else if (instr == ILLEGAL_MOVE_INSTR)
    {
        // Still player's turn; robot will not move.
        message[0] = start_instr_op_len[0];
        message[1] = start_instr_op_len[1];
        rpi_receive(check_bytes, 2);
        if (!utils_validate_transmission(message, 2, check_bytes))
        {
            // Checksum error; corrupted data
        }
    }
    else
    {
        // Error occurred; found no matching instruction
    }
#endif /* KEENAN_TEST */

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
     command_queue_push((command_t*)gantry_robot_build_command());
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
 * @brief Build a gantry_home command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_home_build_command()
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*)malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_home_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &gantry_home_is_done;

    // Data
    p_command->move.source_file = FILE_ERROR;
    p_command->move.source_rank = RANK_ERROR;
    p_command->move.dest_file   = FILE_ERROR;
    p_command->move.dest_rank   = RANK_ERROR;
    p_command->move.move_type   = IDLE;

    return p_command;
}

/**
 * @brief Toggles the homing flag
 *
 * @param command The gantry command being run
 */
void gantry_home_entry(command_t* command)
{
    gantry_homing = !gantry_homing;
}

/**
 * @brief Homing flag is toggled in entry, so return true always
 *
 * @param command The gantry command being run
 * @return true Always
 */
bool gantry_home_is_done(command_t* command)
{
    return true;
}


/**
 * @brief Homes the gantry system (motors all the way up, right, back from point-of-view of the robot)
 */
void gantry_home()
{
    // Set the homing flag
    command_queue_push((command_t*)gantry_home_build_command());

    // Home the motors
    //  TODO: Place two relative commands on the queue. First is Z. Second is X and Y.
    command_queue_push((command_t*)stepper_build_home_command());
    command_queue_push((command_t*)delay_build_command(100));

    // Back away from the edge
    command_queue_push  // TODO: Place two relative commands on the queue. First is Z. Second is X and Y.
    (
            (command_t*)stepper_build_rel_command
            (
                    -6,
                    6,
                    0,
                    1,                                   // v_x
                    1,                                   // v_y
                    0                                    // v_z
            )
    );

    // Clear the homing flag
    command_queue_push((command_t*)gantry_home_build_command());
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
//        gantry_kill();
    }

    // If a limit switch was pressed, disable the appropriate motor
    gantry_limit_stop(switch_data);

    // If the start/reset button was pressed, send the appropriate "new game" signal
    if (switch_data & BUTTON_START_RESET)
    {
//        gantry_reset();
    }

    // If the home button was pressed, clear the queue and execute a homing command
    if (switch_data & BUTTON_HOME)
    {
//        gantry_home();
    }
}

/* End gantry.c */
