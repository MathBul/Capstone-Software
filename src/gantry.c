/**
 * @file gantry.c
 * @author Nick Cooney (npc4crc@virginia.edu), Eli Jelesko (ebj5hec@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Code to unite all other modules
 * @version 1.0
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 */

#include "gantry.h"

// Private functions
static void gantry_kill(void);
static void gantry_estop(void);

// Stores the board readings, which are read in an interrupt and used in various commands
uint64_t board_reading_current      = 0;
uint64_t board_reading_intermediate = 0;

// Flags
bool sys_fault                 = false;
bool sys_reset                 = false;
bool sys_limit                 = false;
static bool gantry_homing      = false;
static bool human_move_legal   = true;
static bool human_move_capture = false;
static bool human_move_done    = false;
static bool initial_valid      = false;
static bool msg_ready_to_send  = true;
static bool robot_is_done      = false;

#ifdef THREE_PART_MODE
static bool ready_to_read      = false;
#endif

/**
 * @brief Initializes all modules
 */
void gantry_init(void)
{
    // System clock and timer initializations
    clock_sys_init();
    clock_timer0a_init();               // X
    clock_timer1a_init();               // Y
    clock_timer2a_init();               // Z
    clock_timer3a_init();               // Switches
    clock_timer4a_init();               // Gantry
    clock_timer5a_init();               // Delay
    clock_timer6a_init();               // LEDs
    clock_timer7c_init();               // Comm delay
    clock_start_timer(GANTRY_TIMER);

    // System level initialization of all other modules
    command_queue_init();
    led_init();
    rpi_init();
    chessboard_init();
    stepper_init_motors();
    switch_init();

#ifdef PERIPHERALS_ENABLED
    sensornetwork_init();
    electromagnet_init();
#endif

#ifdef THREE_PARTY_MODE
    uart_init(UART_CHANNEL_0);
#endif
}

/**
 * @brief Homes the gantry system (motors all the way up, right, back -- from point-of-view of the robot)
 */
void gantry_home(void)
{
    // Set the homing flag
    command_queue_push((command_t*) gantry_home_build_command());

    // Home the motors with delay
    command_queue_push((command_t*) stepper_build_home_z_command());
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
 * @brief Hard stops the gantry system. Kills (but does not home) motors, does NOT set sys_fault flag
 */
static void gantry_kill(void)
{
    // Disable all motors
    stepper_x_stop();
    stepper_y_stop();
    stepper_z_stop();

    // Turn on the error LED
    led_mode(LED_ERROR);

    // Clear the command queue (just in case)
    command_queue_clear();
}

/**
 * @brief Hard stops the gantry system. Kills (but does not home) motors, sets sys_fault flag
 */
static void gantry_estop(void)
{
    gantry_kill();

    // Set the system fault flag
    sys_fault = true;
}

/* Command Functions */

/**
 * @brief Build a start state validator command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_start_state_build_command(void)
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*) malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_start_state_entry;
    p_command->command.p_action  = &gantry_start_state_action;
    p_command->command.p_exit    = &gantry_start_state_exit;
    p_command->command.p_is_done = &gantry_start_state_is_done;

    return (gantry_command_t*) p_command;
}

/**
 * @brief Done when the start state is valid
 *
 * @returns Pointer to the dynamically-allocated command
 */
void gantry_start_state_entry(command_t* command)
{
    initial_valid = false;
}

/**
 * @brief Resets the entire system (motors, stored chess boards, UART, etc.). NOT a system fault
 *
 * @returns Pointer to the dynamically-allocated command
 */
void gantry_start_state_action(command_t* command)
{
    // Read the board's initial state
    uint64_t initial_presence = sensornetwork_get_reading();
    uint64_t initial_presence_white = (initial_presence & (~INITIAL_PRESENCE_BLACK));
    uint64_t initial_presence_black = (initial_presence & (~INITIAL_PRESENCE_WHITE));

    // Check for an invalid state
    if (initial_presence_white != INITIAL_PRESENCE_WHITE)
    {
        initial_valid = false;
        led_mode(LED_SCANNING_ERROR_WHITE);
    }
    else if (initial_presence_black != INITIAL_PRESENCE_BLACK)
    {
        initial_valid = false;
        led_mode(LED_SCANNING_ERROR_BLACK);
    }
    else
    {
        initial_valid = true;
        led_mode(LED_HUMAN_MOVE);
    }
}

/**
 * @brief Clear the start state valid flag for the next game
 *
 * @returns Pointer to the dynamically-allocated command
 */
void gantry_start_state_exit(command_t* command)
{
    initial_valid = false;
}

/**
 * @brief Done when the start state is valid
 *
 * @returns Pointer to the dynamically-allocated command
 */
bool gantry_start_state_is_done(command_t* command)
{
    return initial_valid;
}

/**
 * @brief Build a reset command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_reset_build_command(void)
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*) malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_reset_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &gantry_reset_is_done;

    return (gantry_command_t*) p_command;
}

/**
 * @brief Resets the entire system (motors, stored chess boards, UART, etc.). NOT a system fault
 *
 * @returns Pointer to the dynamically-allocated command
 */
void gantry_reset_entry(command_t* command)
{
    // Clear the command queue
    command_queue_clear();

    // Indicate that the Pi's not up yet
    led_mode(LED_ROBOT_MOVE);

    // Home the motors
    gantry_home();

    // Reset the chess board
    chessboard_reset_all();

    // Reset the rpi
    rpi_reset_uart();

    // Read the user color
    char user_color = 'W';

    // Clear flags
    sys_limit = false;
    sys_reset = false;

#ifdef FINAL_IMPLEMENTATION_MODE
    // Force an interrupt to fire
    clock_trigger_interrupt(SWITCH_TIMER);
    uint16_t switch_data = switch_get_reading();

    // Wait for a valid start state
    command_queue_push((command_t*) gantry_start_state_build_command());

    // Start the game
    if (switch_data & TOGGLE_MASK)
    {
        // User is black, start in gantry_robot
        user_color = 'W';

        char message[START_INSTR_LENGTH];
        rpi_build_start_msg(user_color, message);
        command_queue_push((command_t*) gantry_comm_build_command(message, START_INSTR_LENGTH));

        // After receiving an ACK, goto human command
        command_queue_push((command_t*) gantry_human_build_command());
    } else {
        // User is white, start in gantry_human
        user_color = 'B';

        char message[START_INSTR_LENGTH];
        rpi_build_start_msg(user_color, message);
        command_queue_push((command_t*) gantry_comm_build_command(message, START_INSTR_LENGTH));

        // After receiving an ACK, goto robot command
        command_queue_push((command_t*) gantry_robot_build_command());

        // Do not check for a valid initial state
        human_move_legal = true;
    }

#elif defined(THREE_PARTY_MODE)
    uart_reset(USER_CHANNEL);

    // User is always white, start in gantry_human
    user_color = 'W';

    char message[START_INSTR_LENGTH];
    rpi_build_start_msg(user_color, message);
    command_queue_push((command_t*) gantry_comm_build_command(message, START_INSTR_LENGTH));

    // After receiving an ACK, goto human command
    command_queue_push((command_t*) gantry_human_build_command());
#endif
}

/**
 * @brief Always done
 *
 * @returns Pointer to the dynamically-allocated command
 */
bool gantry_reset_is_done(command_t* command)
{
    return true;
}

/**
 * @brief Build a gantry_human command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_command_t* gantry_human_build_command(void)
{
#ifdef FINAL_IMPLEMENTATION_MODE
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*) malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_human_entry;
    p_command->command.p_action  = &gantry_human_action;
    p_command->command.p_exit    = &gantry_human_exit;
    p_command->command.p_is_done = &gantry_human_is_done;

#elif defined(THREE_PARTY_MODE)
    // The thing to return
    gantry_robot_command_t* p_command = (gantry_robot_command_t*) malloc(sizeof(gantry_robot_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_human_entry;
    p_command->command.p_action  = &gantry_human_action;
    p_command->command.p_exit    = &gantry_human_exit;
    p_command->command.p_is_done = &gantry_human_is_done;

    // Data
    p_command->move.source_file = FILE_ERROR;
    p_command->move.source_rank = RANK_ERROR;
    p_command->move.dest_file   = FILE_ERROR;
    p_command->move.dest_rank   = RANK_ERROR;
    p_command->move.move_type   = IDLE;
    p_command->game_status      = ONGOING;

    p_command->move_uci[0] = 255;
    p_command->move_uci[1] = 255;
    p_command->move_uci[2] = 255;
    p_command->move_uci[3] = 255;
    p_command->move_uci[4] = 255;
#endif

    return (gantry_command_t*) p_command;
}

/*
 * @brief Clear flags
 *
 * @param command The gantry command being run
 */
void gantry_human_entry(command_t* command)
{
    if (!human_move_legal)
    {
        led_mode(LED_ILLEGAL);
    }
    else
    {
        led_mode(LED_HUMAN_MOVE);
    }

    // Reset the flags
    human_move_capture = false;
    human_move_done    = false;

#ifdef THREE_PARTY_MODE
    ready_to_read      = false;
#endif
}

/*
 * @brief Reads the move the human has made until the human hits the "end turn" button
 *
 * @param command The gantry command being run
 */
void gantry_human_action(command_t* command)
{
#ifdef FINAL_IMPLEMENTATION_MODE

#elif defined(THREE_PARTY_MODE)
    if (!ready_to_read) {
        return;
    }

    gantry_robot_command_t* p_gantry_command = (gantry_robot_command_t*) command; // WHY IS THIS A ROBOT COMMAND!?!?!

    char message[9];
    char move[5];
    char check_bytes[2];
    bool msg_status = false;

    // Read the START byte
    msg_status = uart_read_byte(USER_CHANNEL, (uint8_t*) &message[0]);
    if ((!msg_status) || (message[0] != START_BYTE))
    {
        return;
    }

    // Read the INSTRUCTION byte
    msg_status = uart_read_byte(USER_CHANNEL, (uint8_t*) &message[1]);
    uint8_t instruction = message[1] >> 4;                          // Shift to remove the LENGTH portion from this section of this message
    if ((!msg_status) || (instruction != HUMAN_MOVE_INSTR))
    {
        // If the RPi responded "illegal move", receive the rest of the message, then short circuit to robot_is_done
        if (instruction == ILLEGAL_MOVE_INSTR)
        {
            // Read the CHECK BYTES data and validate the transmission
            msg_status = uart_read_string(USER_CHANNEL, check_bytes, 2);
            if ((!msg_status) || (!utils_validate_transmission((uint8_t *) message, 2, check_bytes)))
            {
                return;
            }

            // No ACK's

            // Turn on the error LED
            led_mode(LED_ERROR);

            // Mark the humans's move as illegal, the robot's move as done
            human_move_legal = false;
            p_gantry_command->move.move_type = IDLE;
            robot_is_done = true;
        }
        return;
    }

    // Read the MOVE bytes
    msg_status = uart_read_string(USER_CHANNEL, move, 5);
    if (!msg_status)
    {
        return;
    }
    message[2] = move[0];
    message[3] = move[1];
    message[4] = move[2];
    message[5] = move[3];
    message[6] = move[4];

    // No GAME STATUS byte

    // Read the CHECK BYTES data and validate the transmission
    msg_status = uart_read_string(USER_CHANNEL, &check_bytes[0], 2);
    if ((!msg_status) || (!utils_validate_transmission((uint8_t *) message, 7, check_bytes)))
    {
        return;
    }

    // At this point, the full message was received properly. Copy the check bytes and transmit to the RPi    
    message[7] = check_bytes[0];
    message[8] = check_bytes[1];

    // Store the UCI for the Comm command
    p_gantry_command->move_uci[0] = move[0];
    p_gantry_command->move_uci[1] = move[1];
    p_gantry_command->move_uci[2] = move[2];
    p_gantry_command->move_uci[3] = move[3];
    p_gantry_command->move_uci[4] = move[4];

    human_move_done = true;
#endif
}

/**
 * @brief Places a human or comm command on the queue depending on if the detected move was legal
 *
 * @param command The gantry command being run
 */
void gantry_human_exit(command_t* command)
{
#ifdef FINAL_IMPLEMENTATION_MODE
    // Make sure a reset has not been issued
    if (sys_reset || sys_limit)
    {
        return;
    }

    // Update the board state from the reading
    human_move_legal = true;
    char move[5];

    if (human_move_capture)
    {
        human_move_legal &= chessboard_update_intermediate_board_from_presence(board_reading_intermediate, move);
    }
    human_move_legal &= chessboard_update_current_board_from_presence(board_reading_current, move, human_move_capture);

    // If the move was roughly legal, prepare to transmit. Otherwise, turn on the error LED and wait for a new move
    if (human_move_legal)
    {
        // Place the gantry_comm command on the queue to send the message
        char message[HUMAN_MOVE_INSTR_LENGTH];
        rpi_build_human_move_msg(move, message);
        command_queue_push((command_t*) gantry_comm_build_command(message, HUMAN_MOVE_INSTR_LENGTH));
        command_queue_push((command_t*) gantry_robot_build_command());

        // Prepare to send the COMM message
        msg_ready_to_send = true;
    }
    else
    {
        // Change the LED's to indicate an error
        led_mode(LED_ERROR);
        
        // Place the gantry_human command on the queue until a legal move is given
        command_queue_push((command_t*) gantry_human_build_command());

        // Clear the flags
        human_move_capture = false;
    }

#elif defined(THREE_PARTY_MODE)
    gantry_robot_command_t* p_gantry_command = (gantry_robot_command_t*) command;

    // Update local board state
    chessboard_update_current_board_from_previous_board();
    chessboard_update_current_board_from_move(p_gantry_command->move_uci);

    // Place the gantry_comm command on the queue to send the message
    char message[HUMAN_MOVE_INSTR_LENGTH];
    rpi_build_human_move_msg(p_gantry_command->move_uci, message);
    command_queue_push((command_t*) gantry_comm_build_command(message, HUMAN_MOVE_INSTR_LENGTH));
    command_queue_push((command_t*) gantry_robot_build_command());

    // Prepare to send the COMM message
    human_move_legal = true;
    msg_ready_to_send = true;
#endif
}

/**
 * @brief Moves to the next command once the END_TURN button has been pressed
 * 
 * @param command The gantry command being run
 * @return true If the END_TURN button has been pressed, false otherwise
 */
bool gantry_human_is_done(command_t* command)
{
    return human_move_done;
}

/**
 * @brief Build a gantry_comm command
 *
 * @param move The move, in UCI notation, to send to the RPi
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_comm_command_t* gantry_comm_build_command(char* message, uint8_t message_length)
{
    // The thing to return
    gantry_comm_command_t* p_command = (gantry_comm_command_t*) malloc(sizeof(gantry_comm_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_comm_entry;
    p_command->command.p_action  = &gantry_comm_action;
    p_command->command.p_exit    = &gantry_comm_exit;
    p_command->command.p_is_done = &gantry_comm_is_done;

    // The move to be sent
    uint8_t i = 0;
    for (i = 0; i < message_length; i++)
    {
        p_command->message[i] = message[i];
    }
    p_command->message_length = message_length;

    return p_command;
}

/*
 * @brief Sends the supplied message once to guarantee it sends
 *
 * @param command The gantry command being run
 */
void gantry_comm_entry(command_t* command)
{
    gantry_comm_command_t* p_gantry_command = (gantry_comm_command_t*) command;

    // Indicate we're talking to the pi
    led_mode(LED_WAITING_FOR_MSG);

    // Send the message
    rpi_transmit(p_gantry_command->message, p_gantry_command->message_length);

    // Do not resend the message until the interrupt sets send_msg
    msg_ready_to_send = false;

    // Start the timer
    clock_reset_timer_value(COMM_TIMER);
    clock_start_timer(COMM_TIMER);
}

/*
 * @brief Sends the supplied message (every 5 seconds) until an ACK is received from the RPi
 *
 * @param command The gantry command being run
 */
void gantry_comm_action(command_t* command)
{
    gantry_comm_command_t* p_gantry_command = (gantry_comm_command_t*) command;

    // Message is ready for first try and every 5 seconds after
    if (msg_ready_to_send)
    {
        // Send the message
        rpi_transmit(p_gantry_command->message, p_gantry_command->message_length);

        // Do not resend the message until the interrupt sets send_msg
        msg_ready_to_send = false;

        // Start the timer
        clock_start_timer(COMM_TIMER);
    }
}

/**
 * @brief Places a robot command command on the queue
 *
 * @param command The gantry command being run
 */
void gantry_comm_exit(command_t* command)
{
    // Stop and reset the timer
    clock_stop_timer(COMM_TIMER);
    clock_reset_timer_value(COMM_TIMER);
}

/**
 * @brief Checks if an ACK has been received from the RPi
 * 
 * @param command The gantry command being run
 * @return true If an ack_byte has been received, false otherwise
 */
bool gantry_comm_is_done(command_t* command)
{
    char ack_byte = '\0';
    bool got_byte = false;

    // Read from the Pi
    got_byte = rpi_receive_unblocked(&ack_byte, 1);

    // If we get an ACK, we are done
    return ((ack_byte == ACK_BYTE) && got_byte);
}

/**
 * @brief Build a gantry_robot command
 *
 * @returns Pointer to the dynamically-allocated command
 */
gantry_robot_command_t* gantry_robot_build_command(void)
{
    // The thing to return
    gantry_robot_command_t* p_command = (gantry_robot_command_t*) malloc(sizeof(gantry_robot_command_t));

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
    p_command->game_status      = ONGOING;

    p_command->move_uci[0] = 0;
    p_command->move_uci[1] = 0;
    p_command->move_uci[2] = 0;
    p_command->move_uci[3] = 0;
    p_command->move_uci[4] = 0;

    return p_command;
}

/**
 * @brief Prepares the gantry for a move command. Nothing is done in this case.
 * 
 * @param command The gantry command being run
 */
void gantry_robot_entry(command_t* command)
{
    gantry_robot_command_t* gantry_robot_move_cmd = (gantry_robot_command_t*) command;

    // Set the robot moving LED
    led_mode(LED_ROBOT_MOVE);

    // Reset everything
    robot_is_done = false;
    gantry_robot_move_cmd->move.source_file = FILE_ERROR;
    gantry_robot_move_cmd->move.source_rank = RANK_ERROR;
    gantry_robot_move_cmd->move.dest_file   = FILE_ERROR;
    gantry_robot_move_cmd->move.dest_rank   = RANK_ERROR;
    gantry_robot_move_cmd->move.move_type   = IDLE;
}

/**
 * @brief Reads from the RPi until data has been received
 * 
 * @param command The gantry command being run
 */
void gantry_robot_action(command_t* command)
{
    gantry_robot_command_t* p_gantry_command = (gantry_robot_command_t*) command;
    uint8_t status_after_human = 0;
    uint8_t status_after_robot = 0;
    char move[5];
    char message[8];
    char check_bytes[2];
    bool msg_status = false;

    // Read the START byte
    msg_status = rpi_receive(&message[0], 1);
    if ((!msg_status) || (message[0] != START_BYTE))
    {
        return;
    }

    // Read the INSTRUCTION byte
    msg_status = rpi_receive(&message[1], 1);
    uint8_t instruction = message[1] >> 4;                          // Shift to remove the LENGTH portion from this section of this message
    if ((!msg_status) || (instruction != ROBOT_MOVE_INSTR))
    {
        // If the RPi responded "illegal move", receive the rest of the message, then short circuit to robot_is_done
        if (instruction == ILLEGAL_MOVE_INSTR)
        {
            // Read the CHECK BYTES data and validate the transmission
            msg_status = rpi_receive(check_bytes, 2);
            if ((!msg_status) || (!utils_validate_transmission((uint8_t *) message, 2, check_bytes)))
            {
                return;
            }

            // Transmit an ACK
            rpi_transmit_ack();

            // Turn on the error LED
            led_mode(LED_ERROR);

            // Mark the humans's move as illegal, the robot's move as done
            p_gantry_command->move.move_type = IDLE;
            human_move_legal = false;
            robot_is_done = true;
        }
        return;
    }

    // Read the MOVE bytes
    msg_status = rpi_receive(move, 5);
    if (!msg_status)
    {
        return;
    }
    message[2] = move[0];
    message[3] = move[1];
    message[4] = move[2];
    message[5] = move[3];
    message[6] = move[4];

    // Read the GAME STATUS byte
    msg_status = rpi_receive(&message[7], 1);
    if (!msg_status)
    {
        return;
    }

    // Read the CHECK BYTES data and validate the transmission
    msg_status = rpi_receive(&check_bytes[0], 2);
    if ((!msg_status) || (!utils_validate_transmission((uint8_t *) message, 8, check_bytes)))
    {
        return;
    }

    // At this point, the full message was received properly. Transmit an ACK
    rpi_transmit_ack();

    // Since the human move was legal, we can update the previous board 
    chessboard_update_previous_board_from_current_board();

    // To reduce the number of transmissions, game_status holds the status after the last human move and (possibly) the resulting robot move
    char game_status = message[7];
    status_after_human = (game_status >> 4);
    status_after_robot = (game_status & 0x0F);

    // Record the UCI move
    p_gantry_command->move_uci[0] = move[0];
    p_gantry_command->move_uci[1] = move[1];
    p_gantry_command->move_uci[2] = move[2];
    p_gantry_command->move_uci[3] = move[3];
    p_gantry_command->move_uci[4] = move[4];

    // Interpret the status of the game
    if (status_after_human == GAME_CHECKMATE)
    {
        // If the human ended the game, do not let the robot move
        p_gantry_command->game_status    = HUMAN_WIN;
        p_gantry_command->move.move_type = IDLE;
    }
    else if (status_after_robot == GAME_CHECKMATE)
    {
        // If the robot is about to win the game, let it make the final move
        p_gantry_command->game_status      = ROBOT_WIN;
        p_gantry_command->move.source_file = utils_byte_to_file(move[0]);
        p_gantry_command->move.source_rank = utils_byte_to_rank(move[1]);
        p_gantry_command->move.dest_file   = utils_byte_to_file(move[2]);
        p_gantry_command->move.dest_rank   = utils_byte_to_rank(move[3]);
        p_gantry_command->move.move_type   = utils_byte_to_move_type(move[4]);
    }
    else if (status_after_human == GAME_STALEMATE)
    {
        // If the human put the game in stalemate, do not let the robot move
        p_gantry_command->game_status    = STALEMATE;
        p_gantry_command->move.move_type = IDLE;
    }
    else if (status_after_robot == GAME_STALEMATE)
    {
        // If the robot is about to put the game in stalemate, let it make the final move
        p_gantry_command->game_status      = STALEMATE;
        p_gantry_command->move.source_file = utils_byte_to_file(move[0]);
        p_gantry_command->move.source_rank = utils_byte_to_rank(move[1]);
        p_gantry_command->move.dest_file   = utils_byte_to_file(move[2]);
        p_gantry_command->move.dest_rank   = utils_byte_to_rank(move[3]);
        p_gantry_command->move.move_type   = utils_byte_to_move_type(move[4]);
    }
    else
    {
        // If both moves continue the game, proceed as usual
        p_gantry_command->game_status      = ONGOING;
        p_gantry_command->move.source_file = utils_byte_to_file(move[0]);
        p_gantry_command->move.source_rank = utils_byte_to_rank(move[1]);
        p_gantry_command->move.dest_file   = utils_byte_to_file(move[2]);
        p_gantry_command->move.dest_rank   = utils_byte_to_rank(move[3]);
        p_gantry_command->move.move_type   = utils_byte_to_move_type(move[4]);
    }

    robot_is_done = true;
}

/**
 * @brief Helper function to move the specified piece from the initial position to the final position
 * 
 * @param initial_file The initial position file
 * @param initial_rank The initial position rank
 * @param final_file The final position file
 * @param final_rank The final position rank
 * @param piece The piece being moved
 */
void gantry_robot_move_piece(chess_file_t initial_file, chess_rank_t initial_rank, chess_file_t final_file, chess_rank_t final_rank, chess_piece_t piece)
{
    // Check for errors
    if ((initial_file == FILE_ERROR) || (initial_rank == RANK_ERROR) || (final_file == FILE_ERROR) || (final_rank == RANK_ERROR))
    {
        return;
    }

    // Go to the source tile
    command_queue_push((command_t*) stepper_build_chess_xy_command(initial_file, initial_rank, MOTORS_MOVE_V_X, MOTORS_MOVE_V_Y));

#ifdef PERIPHERALS_ENABLED
    // Engage the magnet
    command_queue_push((command_t*) electromagnet_build_command(enabled));
#endif

    // Lower the magnet
    command_queue_push((command_t*) stepper_build_chess_z_command(piece, MOTORS_MOVE_V_Z));

    // Wait
    command_queue_push((command_t*) delay_build_command(1000));

    // Raise the magnet
    command_queue_push((command_t*) stepper_build_chess_z_command(HOME_PIECE, MOTORS_MOVE_V_Z));

    // Go to the destination tile
    command_queue_push((command_t*) stepper_build_chess_xy_command(final_file, final_rank, MOTORS_MOVE_V_X, MOTORS_MOVE_V_Y));

    // Lower the magnet
    command_queue_push((command_t*) stepper_build_chess_z_command(piece, MOTORS_MOVE_V_Z));

#ifdef PERIPHERALS_ENABLED
    // Disengage the magnet
    command_queue_push((command_t*) electromagnet_build_command(disabled));
#endif

    // Wait
    command_queue_push((command_t*) delay_build_command(500));

    // Raise the magnet
    command_queue_push((command_t*) stepper_build_chess_z_command(HOME_PIECE, MOTORS_MOVE_V_Z));
}

/**
 * @brief Interprets the RPi's move, and adds the corresponding commands to the queue
 *
 * @param command The gantry command being run
 */
void gantry_robot_exit(command_t* command)
{
    gantry_robot_command_t* p_gantry_command = (gantry_robot_command_t*) command;

    // Make sure a reset has not been issued
    if (sys_reset || sys_limit)
    {
        return;
    }

    // Special case of human made an illegal move
    if (!human_move_legal)
    {
        // Turn on the error LED and go back to human move
        led_mode(LED_ERROR);
        command_queue_push((command_t*) gantry_human_build_command());
        return;
    }
    
    // Load commands based on the move that the RPi sent
    chess_move_t rook_move;
    chess_piece_t moving_piece;

    switch (p_gantry_command->move.move_type)
    {
        case MOVE:
            // Make the move
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case PROMOTION:
            // Banish the source pawn to the graveyard
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                CAPTURE_FILE,
                CAPTURE_RANK,
                moving_piece
            );

            // Revive the queen from the magical **queen tile** and move it to the destination
            moving_piece = QUEEN;
            gantry_robot_move_piece(
                QUEEN_FILE,
                QUEEN_RANK,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case CAPTURE_PROMOTION:
            // Banish the piece being captured to the graveyard
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.dest_file, p_gantry_command->move.dest_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                CAPTURE_FILE,
                CAPTURE_RANK,
                moving_piece
            );

            // Banish the piece being captured to the graveyard
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                CAPTURE_FILE,
                CAPTURE_RANK,
                moving_piece
            );

            // Revive the queen from the magical **queen tile** and move it to the destination
            moving_piece = QUEEN;
            gantry_robot_move_piece(
                QUEEN_FILE,
                QUEEN_RANK,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case CAPTURE:
            // Banish the piece being captured to the graveyard
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.dest_file, p_gantry_command->move.dest_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                CAPTURE_FILE,
                CAPTURE_RANK,
                moving_piece
            );

            // Make the move
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case CASTLING:
            // Move the king
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // UCI notation gives us the king's move, determine the rook's move
            rook_move = rpi_castle_get_rook_move(&p_gantry_command->move);

            // Move the rook
            moving_piece = ROOK;
            gantry_robot_move_piece(
                rook_move.source_file,
                rook_move.source_rank,
                rook_move.dest_file,
                rook_move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case EN_PASSENT:
            // With an en passant capture, the captured pawn will have the moving pawn's *source rank* and *destination file*
            
            // Banish the piece being captured to the graveyard
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.dest_file, p_gantry_command->move.dest_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.dest_file,
                p_gantry_command->move.source_rank,
                CAPTURE_FILE,
                CAPTURE_RANK,
                moving_piece
            );

            // Make the move
            moving_piece = chessboard_get_piece_at_position(p_gantry_command->move.source_file, p_gantry_command->move.source_rank);
            gantry_robot_move_piece(
                p_gantry_command->move.source_file,
                p_gantry_command->move.source_rank,
                p_gantry_command->move.dest_file,
                p_gantry_command->move.dest_rank,
                moving_piece
            );

            // Go to home
            gantry_home();
        break;

        case IDLE:
            // Move was invalid, do nothing
        break;

        default:
            // Move was invalid, do nothing
        break;
    }

    // Check if the game is still going
    switch (p_gantry_command->game_status) 
    {
        case ONGOING:
            command_queue_push((command_t*) gantry_human_build_command());
        break;

        case HUMAN_WIN:
            // Turn on a flashing green LED
            led_mode(LED_HUMAN_WIN);
        break;

        case ROBOT_WIN:
            // Turn on a flashing blue LED
            led_mode(LED_ROBOT_WIN);
        break;

        case STALEMATE:
            // Turn on a flashing white LED
            led_mode(LED_STALEMATE);
        break;
    }

    // Finally, update previous_board with the robot's move
    chessboard_update_previous_board_from_move(p_gantry_command->move_uci);
}

/**
 * @brief Moves to the next command once a valid move has been received from the RPi
 * 
 * @param command The gantry command being run
 * @return true If the RPi responded and the transmission was valid
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
gantry_command_t* gantry_home_build_command(void)
{
    // The thing to return
    gantry_command_t* p_command = (gantry_command_t*) malloc(sizeof(gantry_command_t));

    // Functions
    p_command->command.p_entry   = &gantry_home_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &gantry_home_is_done;

    return p_command;
}

/**
 * @brief Toggles the homing flag
 *
 * @param command The gantry command being run
 */
void gantry_home_entry(command_t* command)
{
    led_mode(LED_ROBOT_MOVE);
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

/* Interrupts */

/**
 * @brief Interrupt handler for the gantry module
 */
__interrupt void GANTRY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(GANTRY_TIMER);
    
    // Check the current switch readings
    uint16_t switch_data = switch_get_reading();

    // If the emergency stop button was pressed, kill everything
    if (switch_data & E_STOP_MASK)
    {
        gantry_estop();
    }

    // If a limit switch was pressed, and the system is not homing, kill everything
    if ((!sys_limit) && (!gantry_homing) && (switch_data & LIMIT_MASK))
    {
        sys_limit = true;
        gantry_kill();
    }

    // If the start/reset/home button was pressed, send the appropriate "new game" signal
    if ((!sys_reset) && (switch_data & (BUTTON_RESET_MASK | BUTTON_START_MASK | BUTTON_HOME_MASK)))
    {
        sys_reset = true;
        command_queue_push((command_t*) gantry_reset_build_command());
    }

    // Store the current reading if the human hit the capture tile
    if ((!human_move_capture) && (switch_data & SWITCH_CAPTURE_MASK))
    {
        human_move_capture = true;
        board_reading_intermediate = sensornetwork_get_reading();
        led_mode(LED_CAPTURE);
    }

#ifdef FINAL_IMPLEMENTATION_MODE
    // Store the current reading if the human hit the "end turn"" tile
    if ((!human_move_done) && (switch_data & BUTTON_NEXT_TURN_MASK))
    {
        board_reading_current = sensornetwork_get_reading();
        human_move_done = true;
    }

#elif defined(THREE_PARTY_MODE)
    if ((!human_move_done) && (switch_data & BUTTON_NEXT_TURN_MASK))
    {
        ready_to_read = true;
    }
#endif
}

/**
 * @brief Interrupt handler for the 5-second communication timer
 */
__interrupt void COMM_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(COMM_TIMER);

    // Indicate that the message timed out
    msg_ready_to_send = true;
}

/* End gantry.c */
