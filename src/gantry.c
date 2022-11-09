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

//#define PERIPHERALS_ENABLED

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
static bool gantry_homing = false;
static bool robot_is_done = false;
static bool human_is_done = false;
static bool comm_error = false;
extern bool utils_sys_fault;

/**
 * @brief Initializes all modules
 */
void gantry_init()
{
    // System clock and timer initializations
    clock_sys_init();
    clock_timer0a_init();               // X
    clock_timer1a_init();               // Y
    clock_timer2a_init();               // Z
    clock_timer3a_init();               // Switches
    clock_timer4a_init();               // Gantry
    clock_timer5a_init();               // Delay
    clock_start_timer(GANTRY_TIMER);

    // System level initialization of all other modules
    command_queue_init();
#ifdef PERIPHERALS_ENABLED
    electromagnet_init();
    leds_init();
    sensors_init();
#endif /* PERIPHERALS_ENABLED */
    switch_init();
    stepper_init_motors();
    rpi_init();
#ifdef THREE_PARTY_MODE
    uart_init(UART_CHANNEL_0);
#endif
}

/**
 * @brief Homes the system then adds a robot command to the queue
 */
void gantry_start()
{
    gantry_home();
#if defined(FINAL_IMPLEMENTATION_MODE) || defined(THREE_PARTY_MODE)
    rpi_transmit_start('W');
    command_queue_push((command_t*)gantry_human_build_command());
#else
    command_queue_push((command_t*) gantry_robot_build_command());
#endif
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
#ifdef THREE_PARTY_MODE
    uart_reset(USER_CHANNEL);
#endif
#ifdef FINAL_IMPLEMENTATION_MODE
    // Start a new game
    char user_color = 'W';
    uint8_t switch_data = switch_get_reading();
    if (switch_data & ROCKER_COLOR)
    {
        user_color = 'B';
    }
    rpi_transmit_start(user_color);
#endif
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
    p_command->command.p_action  = &gantry_human_action;
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

void gantry_human_action(command_t* command)
{
#ifdef FINAL_IMPLEMENTATION_MODE
    // Read the current board state
    uint64_t board_reading = sensor_get_reading();

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
#endif

#ifdef THREE_PARTY_MODE
    char first_byte = 0;
    char instr_op_len = 0;
    char move[5];
    char check_bytes[2];
    char pi_message[9];
    uint8_t instr;
    uint8_t op_len;

    // First, read the first two bytes of the entire message from terminal
    if (uart_receive(USER_CHANNEL, &first_byte, 1))
    {
        if (first_byte == START_BYTE)
        {
            if (uart_receive(USER_CHANNEL, &instr_op_len, 1))
            {
                instr = instr_op_len >> 4;
                op_len = instr_op_len & (~0xF0);
                if (instr == HUMAN_MOVE_INSTR)
                {
                    if (uart_receive(USER_CHANNEL, move, 5))
                    {
                        pi_message[0] = START_BYTE;
                        pi_message[1] = HUMAN_MOVE_INSTR_AND_LEN;
                        pi_message[2] = move[0];
                        pi_message[3] = move[1];
                        pi_message[4] = move[2];
                        pi_message[5] = move[3];
                        pi_message[6] = move[4];
                        utils_fl16_data_to_cbytes((uint8_t *) pi_message, 7, check_bytes);
                        pi_message[7] = check_bytes[0];
                        pi_message[8] = check_bytes[1];

                        // Transmit the move to the RPi (9 bytes long)
                        rpi_transmit(pi_message, 9);

                        // We're done!
                        human_is_done = true;
                    }
                }
            }
        }
    }
    else
    {
        // Didn't get anything; move on
    }
#endif
}

/**
 * @brief Interprets the RPi's move, and adds the appropriate commands to the queue
 *  TODO: Incomplete
 *
 * @param command The gantry command being run
 */
void gantry_human_exit(command_t* command)
{
    // Place the gantry_move command on the queue
    command_queue_push((command_t*)gantry_robot_build_command());

    // Reset flag
    human_is_done = false;
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
#ifdef FINAL_IMPLEMENTATION_MODE
    uint8_t switch_data = switch_get_reading();
    return (switch_data & BUTTON_END_TURN);
#elif defined(THREE_PARTY_MODE)
    return human_is_done;
#endif
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
#ifdef USER_MODE
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
#endif /* USER_MODE */

#if defined(KEENAN_TEST) || defined(THREE_PARTY_MODE)
    gantry_command_t* p_gantry_command = (gantry_command_t*) command;
    char first_byte = 0;
    char instr_op_len = 0;
    char move[5];
    char check_bytes[2];
    char message[7];
    uint8_t instr;
    uint8_t op_len;

    // First, read the first two bytes of the entire message
    if (rpi_receive(&first_byte, 1))
    {
        if (first_byte == START_BYTE)
        {
            if (rpi_receive(&instr_op_len, 1))
            {
                instr = instr_op_len >> 4;
                op_len = instr_op_len & (~0xF0);
                if (instr == ROBOT_MOVE_INSTR)
                {
                    if (rpi_receive(move, 5))
                    {
                        p_gantry_command->move.source_file = utils_byte_to_file(move[0]);
                        p_gantry_command->move.source_rank = utils_byte_to_rank(move[1]);
                        p_gantry_command->move.dest_file = utils_byte_to_file(move[2]);
                        p_gantry_command->move.dest_rank = utils_byte_to_rank(move[3]);
                        p_gantry_command->move.move_type = utils_byte_to_move_type(move[4]);
                        message[0] = first_byte;
                        message[1] = instr_op_len;
                        message[2] = move[0];
                        message[3] = move[1];
                        message[4] = move[2];
                        message[5] = move[3];
                        message[6] = move[4];
                        rpi_receive(check_bytes, 2);
                        if (utils_validate_transmission((uint8_t *) message, 7, check_bytes))
                        {
                            // Checksum error; corrupted data
                            robot_is_done = true;
                        }
                    }
                }
                else if (instr == ILLEGAL_MOVE_INSTR)
                {
                    // Still player's turn; robot will not move.
                    message[0] = first_byte;
                    message[1] = instr_op_len;
                    rpi_receive(check_bytes, 2);
                    if (utils_validate_transmission((uint8_t *) message, 2, check_bytes))
                    {
                        p_gantry_command->move.move_type = IDLE;
                        robot_is_done = true;
                    }
                }
            }
        }
    }
#endif /* KEENAN_TEST || THREE_PARTY_MODE */
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
                     EMPTY,                               // piece
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
                     EMPTY,                            // piece
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

             gantry_home();
         break;
         case PROMOTION:
             // TODO
         break;
        
         case CAPTURE:
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
                      p_gantry_command->move.dest_file,  // file
                      p_gantry_command->move.dest_rank,  // rank
                      EMPTY,                               // piece
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
                      CAPTURE_FILE, // file
                      CAPTURE_RANK, // rank
                      EMPTY,                            // piece
                      1,                                // v_x
                      1,                                // v_y
                      0                                 // v_z
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
                      p_gantry_command->move.source_file, // file
                      p_gantry_command->move.source_rank, // rank
                      EMPTY,                            // piece
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

              command_queue_push
              (
                  (command_t*)stepper_build_chess_command
                  (
                      p_gantry_command->move.dest_file, // file
                      p_gantry_command->move.dest_rank, // rank
                      EMPTY,                            // piece
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

              gantry_home();
         break;

         case CASTLING:
             // Error checking
              if (p_gantry_command->move.source_file == FILE_ERROR || p_gantry_command->move.source_rank == RANK_ERROR)
              {
                  break;
              }
              chess_move_t rook_move = rpi_get_castle_rook_move(&p_gantry_command->move);
              // Move to the piece to move
              // the enums are the absolute positions of those ranks/file. current_pos is also absolute
              command_queue_push
              (
                  (command_t*)stepper_build_chess_command
                  (
                      p_gantry_command->move.source_file,  // file
                      p_gantry_command->move.source_rank,  // rank
                      EMPTY,                               // piece
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
                      EMPTY,                            // piece
                      1,                                // v_x
                      1,                                // v_y
                      0                                 // v_z
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
                      rook_move.source_file, // file
                      rook_move.source_rank, // rank
                      EMPTY,                            // piece
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

              command_queue_push
              (
                  (command_t*)stepper_build_chess_command
                  (
                      rook_move.dest_file, // file
                      rook_move.dest_rank, // rank
                      EMPTY,                            // piece
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

              gantry_home();
         break;

         case EN_PASSENT:
             // TODO
         break;

         case IDLE:
             // TODO
             // Invalid move
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
     command_queue_push((command_t*)gantry_human_build_command());
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
 * @brief Homes the gantry system (motors all the way up, right, back -- from point-of-view of the robot)
 */
void gantry_home()
{
    // Set the homing flag
    command_queue_push((command_t*) gantry_home_build_command());

    // Home the motors with delay
//    command_queue_push((command_t*) stepper_build_home_z_command());
    command_queue_push((command_t*) stepper_build_home_xy_command());
    command_queue_push((command_t*) delay_build_command(HOMING_DELAY_MS));

    // Back away from the edge
    command_queue_push((command_t*) stepper_build_rel_command(
            HOMING_X_BACKOFF,
            HOMING_Y_BACKOFF,
            HOMING_Z_BACKOFF,
            HOMING_X_VELOCITY,
            HOMING_Y_VELOCITY,
            HOMING_Z_VELOCITY
    ));

    // Clear the homing flag
    command_queue_push((command_t*) gantry_home_build_command());
}

/**
 * @brief Interrupt handler for the gantry module
 */
__interrupt void GANTRY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(GANTRY_TIMER);
    
    // Check the current switch readings
    uint8_t switch_data = switch_get_reading();

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
