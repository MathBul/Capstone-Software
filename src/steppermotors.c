/**
 * @file steppermotors.c
 * @author Nick Cooney (npc4crc@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Provides functions to control stepper motors driven by DRV8824-Q1 chips
 * @version 0.1
 * @date 2022-10-31
 * 
 * @copyright Copyright (c) 2022
 */

#include "steppermotors.h"

#ifdef STEPPER_DEBUG
#include "uart.h"
#endif

// Private functions
static void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor);
static void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor);
static void stepper_edge_transition(stepper_motors_t *stepper_motor);
static uint16_t stepper_distance_to_transitions(int16_t distance);
static uint32_t stepper_velocity_to_timer_period(uint16_t velocity);
static void stepper_disable_motor(stepper_motors_t *stepper_motor);
static void stepper_disable_all_motors();
static void stepper_enable_motor(stepper_motors_t *stepper_motor);
static void stepper_enable_all_motors();
static void stepper_pause_all_motors();
static void stepper_resume_all_motors();
static int16_t stepper_get_current_pos_mm(stepper_motors_t *p_stepper_motor);
static void stepper_update_velocities(uint16_t v_x, uint16_t v_y, uint16_t v_z, uint16_t max_a_x, uint16_t max_a_y, uint16_t max_a_z);
static uint64_t stepper_get_period_shift(stepper_motors_t* p_stepper_motor);
static void stepper_interrupt_activity(stepper_motors_t *p_stepper_motor);

// Declare the stepper motors
stepper_motors_t stepper_motors[NUMBER_OF_STEPPER_MOTORS];
static stepper_motors_t* p_stepper_motor_x = &stepper_motors[STEPPER_X_ID];
static stepper_motors_t* p_stepper_motor_y = &stepper_motors[STEPPER_Y_ID];
static stepper_motors_t* p_stepper_motor_z = &stepper_motors[STEPPER_Z_ID];

/**
 * @brief Initialize all stepper motors
 */
void stepper_init_motors()
{
    /* Stepper 1 (X-axis) */
    // Disable, set direction clockwise, prepare step
    gpio_set_as_output(STEPPER_X_NENABLE_PORT, STEPPER_X_NENABLE_PIN);
    gpio_set_as_output(STEPPER_X_DIR_PORT, STEPPER_X_DIR_PIN);
    gpio_set_as_output(STEPPER_X_STEP_PORT, STEPPER_X_STEP_PIN);
    gpio_set_output_high(STEPPER_X_NENABLE_PORT, STEPPER_X_NENABLE_PIN);
    gpio_set_output_high(STEPPER_X_DIR_PORT, STEPPER_X_DIR_PIN);
    gpio_set_output_low(STEPPER_X_STEP_PORT, STEPPER_X_STEP_PIN);

    // Set the fault and home indicators as inputs
    gpio_set_as_input(STEPPER_X_NFAULT_PORT, STEPPER_X_NFAULT_PIN);
    gpio_set_as_input(STEPPER_X_NHOME_PORT, STEPPER_X_NHOME_PIN);

    // Configure the stepper struct
    p_stepper_motor_x->timer                      = STEPPER_X_TIMER;
    p_stepper_motor_x->dir_port                   = STEPPER_X_DIR_PORT;
    p_stepper_motor_x->dir_pin                    = STEPPER_X_DIR_PIN;
    p_stepper_motor_x->step_port                  = STEPPER_X_STEP_PORT;
    p_stepper_motor_x->step_pin                   = STEPPER_X_STEP_PIN;
    p_stepper_motor_x->nenable_port               = STEPPER_X_NENABLE_PORT;
    p_stepper_motor_x->nenable_pin                = STEPPER_X_NENABLE_PIN;
    p_stepper_motor_x->nfault_port                = STEPPER_X_NFAULT_PORT;
    p_stepper_motor_x->nfault_pin                 = STEPPER_X_NFAULT_PIN;
    p_stepper_motor_x->nhome_port                 = STEPPER_X_NHOME_PORT;
    p_stepper_motor_x->nhome_pin                  = STEPPER_X_NHOME_PIN;
    p_stepper_motor_x->current_state              = disabled;
    p_stepper_motor_x->transitions_to_desired_pos = 0;
    p_stepper_motor_x->dir                        = 1;
    p_stepper_motor_x->current_pos                = 0;
    p_stepper_motor_x->current_vel                = 0;
#ifdef STEPPER_DEBUG
    p_stepper_motor_x->time_elapsed               = 0;
#endif

    /* Stepper 2 (Y-axis) */
    // Disable, set direction clockwise, prepare step
    gpio_set_as_output(STEPPER_Y_NENABLE_PORT, STEPPER_Y_NENABLE_PIN);
    gpio_set_as_output(STEPPER_Y_DIR_PORT, STEPPER_Y_DIR_PIN);
    gpio_set_as_output(STEPPER_Y_STEP_PORT, STEPPER_Y_STEP_PIN);
    gpio_set_output_high(STEPPER_Y_NENABLE_PORT, STEPPER_Y_NENABLE_PIN);
    gpio_set_output_high(STEPPER_Y_DIR_PORT, STEPPER_Y_DIR_PIN);
    gpio_set_output_low(STEPPER_Y_STEP_PORT, STEPPER_Y_STEP_PIN);

    // Set the fault and home indicators as inputs
    gpio_set_as_input(STEPPER_Y_NFAULT_PORT, STEPPER_Y_NFAULT_PIN);
    gpio_set_as_input(STEPPER_Y_NHOME_PORT, STEPPER_Y_NHOME_PIN);

    // Configure the stepper struct
    p_stepper_motor_y->timer                      = STEPPER_Y_TIMER;
    p_stepper_motor_y->dir_port                   = STEPPER_Y_DIR_PORT;
    p_stepper_motor_y->dir_pin                    = STEPPER_Y_DIR_PIN;
    p_stepper_motor_y->step_port                  = STEPPER_Y_STEP_PORT;
    p_stepper_motor_y->step_pin                   = STEPPER_Y_STEP_PIN;
    p_stepper_motor_y->nenable_port               = STEPPER_Y_NENABLE_PORT;
    p_stepper_motor_y->nenable_pin                = STEPPER_Y_NENABLE_PIN;
    p_stepper_motor_y->nfault_port                = STEPPER_Y_NFAULT_PORT;
    p_stepper_motor_y->nfault_pin                 = STEPPER_Y_NFAULT_PIN;
    p_stepper_motor_y->nhome_port                 = STEPPER_Y_NHOME_PORT;
    p_stepper_motor_y->nhome_pin                  = STEPPER_Y_NHOME_PIN;
    p_stepper_motor_y->current_state              = disabled;
    p_stepper_motor_y->transitions_to_desired_pos = 0;
    p_stepper_motor_y->dir                        = 1;
    p_stepper_motor_y->current_pos                = 0;
    p_stepper_motor_y->current_vel                = 0;
#ifdef STEPPER_DEBUG
    p_stepper_motor_y->time_elapsed               = 0;
#endif

    /* Stepper 3 (Z-axis) */
    // Disable, set direction clockwise, prepare step
    gpio_set_as_output(STEPPER_Z_NENABLE_PORT, STEPPER_Z_NENABLE_PIN);
    gpio_set_as_output(STEPPER_Z_DIR_PORT, STEPPER_Z_DIR_PIN);
    gpio_set_as_output(STEPPER_Z_STEP_PORT, STEPPER_Z_STEP_PIN);
    gpio_set_output_high(STEPPER_Z_NENABLE_PORT, STEPPER_Z_NENABLE_PIN);
    gpio_set_output_high(STEPPER_Z_DIR_PORT, STEPPER_Z_DIR_PIN);
    gpio_set_output_low(STEPPER_Z_STEP_PORT, STEPPER_Z_STEP_PIN);

    // Set the fault and home indicators as inputs
    gpio_set_as_input(STEPPER_Z_NFAULT_PORT, STEPPER_Z_NFAULT_PIN);
    gpio_set_as_input(STEPPER_Z_NHOME_PORT, STEPPER_Z_NHOME_PIN);

    // Configure the stepper struct
    p_stepper_motor_z->timer                      = STEPPER_Z_TIMER;
    p_stepper_motor_z->dir_port                   = STEPPER_Z_DIR_PORT;
    p_stepper_motor_z->dir_pin                    = STEPPER_Z_DIR_PIN;
    p_stepper_motor_z->step_port                  = STEPPER_Z_STEP_PORT;
    p_stepper_motor_z->step_pin                   = STEPPER_Z_STEP_PIN;
    p_stepper_motor_z->nenable_port               = STEPPER_Z_NENABLE_PORT;
    p_stepper_motor_z->nenable_pin                = STEPPER_Z_NENABLE_PIN;
    p_stepper_motor_z->nfault_port                = STEPPER_Z_NFAULT_PORT;
    p_stepper_motor_z->nfault_pin                 = STEPPER_Z_NFAULT_PIN;
    p_stepper_motor_z->nhome_port                 = STEPPER_Z_NHOME_PORT;
    p_stepper_motor_z->nhome_pin                  = STEPPER_Z_NHOME_PIN;    
    p_stepper_motor_z->current_state              = disabled;
    p_stepper_motor_z->transitions_to_desired_pos = 0;
    p_stepper_motor_z->dir                        = 1;
    p_stepper_motor_z->current_pos                = 0;
    p_stepper_motor_z->current_vel                = 0;
#ifdef STEPPER_DEBUG
    p_stepper_motor_z->time_elapsed               = 0;
#endif

    /* Common Stepper GPIO */
    // Configure XY motors for 1/8 stepping
    gpio_set_as_output(STEPPER_XY_MS0_PORT, STEPPER_XY_MS0_PIN);
    gpio_set_as_output(STEPPER_XY_MS1_PORT, STEPPER_XY_MS1_PIN);
    gpio_set_as_output(STEPPER_XY_MS2_PORT, STEPPER_XY_MS2_PIN);
    gpio_set_output_low(STEPPER_XY_MS2_PORT, STEPPER_XY_MS2_PIN);
    gpio_set_output_high(STEPPER_XY_MS1_PORT, STEPPER_XY_MS1_PIN);
    gpio_set_output_high(STEPPER_XY_MS0_PORT, STEPPER_XY_MS0_PIN);

    // Configure Z motor for 1/8 stepping
    gpio_set_as_output(STEPPER_Z_MS0_PORT, STEPPER_Z_MS0_PIN);
    gpio_set_as_output(STEPPER_Z_MS1_PORT, STEPPER_Z_MS1_PIN);
    gpio_set_as_output(STEPPER_Z_MS2_PORT, STEPPER_Z_MS2_PIN);
    gpio_set_output_low(STEPPER_Z_MS2_PORT, STEPPER_Z_MS2_PIN);
    gpio_set_output_high(STEPPER_Z_MS1_PORT, STEPPER_Z_MS1_PIN);
    gpio_set_output_high(STEPPER_Z_MS0_PORT, STEPPER_Z_MS0_PIN);

    // Disable the common reset/sleep
    gpio_set_as_output(STEPPER_XYZ_NRESET_PORT, STEPPER_XYZ_NRESET_PIN);
    gpio_set_output_high(STEPPER_XYZ_NRESET_PORT, STEPPER_XYZ_NRESET_PIN);

    // Place the motors in mixed decay mode (open pin)
    gpio_set_as_output(STEPPER_XYZ_DECAY_PORT, STEPPER_XYZ_DECAY_PIN);
#ifdef STEPPER_DEBUG
    uart_init(PROFILING_CHANNEL);
#endif
}

/**
 * @brief Change the direction of the given stepper motor to clockwise (when looking at the gear)
 * 
 * @param p_stepper_motor The stepper motor to set direction on 
 */
static void stepper_set_direction_clockwise(stepper_motors_t *p_stepper_motor)
{
    gpio_set_output_high(p_stepper_motor->dir_port, p_stepper_motor->dir_pin);
    p_stepper_motor->dir = -1;
}

/**
 * @brief Change the direction of the given stepper motor to counterclockwise (when looking at the gear)
 * 
 * @param p_stepper_motor The stepper motor to toggle direction on
 */
static void stepper_set_direction_counterclockwise(stepper_motors_t *p_stepper_motor)
{
    gpio_set_output_low(p_stepper_motor->dir_port, p_stepper_motor->dir_pin);
    p_stepper_motor->dir = 1;
}

/**
 * @brief Perform an edge transition on the given stepper motor's STEP pin
 * 
 * @param p_stepper_motor The stepper motor to perform the transition on
 */
static void stepper_edge_transition(stepper_motors_t *p_stepper_motor)
{
    gpio_set_output_toggle(p_stepper_motor->step_port, p_stepper_motor->step_pin);
}

/**
 * @brief Convert a distance (mm) to the number of edge transitions required to drive a stepper that far
 *
 * @param distance The distance to travel
 * @return The number of edge transitions to travel that distance
 */
static uint16_t stepper_distance_to_transitions(int16_t distance)
{
    if (distance < 0)
    {
        return (-TRANSITIONS_PER_MM)*distance;
    }
    return TRANSITIONS_PER_MM*distance;
}

/**
 * @brief Converts a velocity (mm/s) to the timer period required to reach it
 *
 * @param velocity The desired velocity (bounded)
 * @return The period for the appropriate timer
 */
static uint32_t stepper_velocity_to_timer_period(uint16_t velocity)
{
    return (SYSCLOCK_FREQUENCY / stepper_distance_to_transitions(velocity));
}

/**
 * @brief Disable the specified stepper and update its current_state
 * 
 * @param p_stepper_motor The stepper motor to disable
 */
static void stepper_disable_motor(stepper_motors_t *p_stepper_motor)
{
    gpio_set_output_high(p_stepper_motor->nenable_port, p_stepper_motor->nenable_pin);
    p_stepper_motor->current_state = disabled;
}

/**
 * @brief Disable all motors
 */
static void stepper_disable_all_motors()
{
    uint8_t i = 0;
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_disable_motor(&stepper_motors[i]);
    }
}

/**
 * @brief Enable the specified stepper and update its current_state
 * 
 * @param p_stepper_motor The stepper motor to enable
 */
static void stepper_enable_motor(stepper_motors_t *p_stepper_motor)
{
    gpio_set_output_low(p_stepper_motor->nenable_port, p_stepper_motor->nenable_pin);
    p_stepper_motor->current_state = enabled;
}

/**
 * @brief Enable all motors
 */
static void stepper_enable_all_motors()
{
    uint8_t i = 0;
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_enable_motor(&stepper_motors[i]);
    }
}

/**
 * @brief Disables the STEPPER_X motor, clears the transitions left, and stops its timer
 */
void stepper_x_stop()
{
    stepper_disable_motor(p_stepper_motor_x);
    p_stepper_motor_x->transitions_to_desired_pos = 0;
    clock_stop_timer(STEPPER_X_TIMER);
}

/**
 * @brief Disables the STEPPER_Y motor, clears the transitions left, and stops its timer
 * 
 */
void stepper_y_stop()
{
    stepper_disable_motor(p_stepper_motor_y);
    p_stepper_motor_y->transitions_to_desired_pos = 0;
    clock_stop_timer(STEPPER_Y_TIMER);
}

/**
 * @brief Disables the STEPPER_Z motor, clears the transitions left, and stops its timer
 * 
 */
void stepper_z_stop()
{
    stepper_disable_motor(p_stepper_motor_z);
    p_stepper_motor_z->transitions_to_desired_pos = 0;
    clock_stop_timer(STEPPER_Z_TIMER);
}

/**
 * @brief Checks if has a fault occured on STEPPER_X_MOTOR
 * 
 * @return Whether a fault occured
 */
bool stepper_x_has_fault()
{
    uint8_t nfault = gpio_read_input(p_stepper_motor_x->nfault_port, p_stepper_motor_x->nfault_pin);

    // Return true (fault) if nfault (active-low) is 0
    return nfault == 0;
}

/**
 * @brief Checks if has a fault occured on STEPPER_Y_MOTOR
 * 
 * @return Whether a fault occured
 */
bool stepper_y_has_fault()
{
    uint8_t nfault = gpio_read_input(p_stepper_motor_y->nfault_port, p_stepper_motor_y->nfault_pin);

    // Return true (fault) if nfault (active-low) is 0
    return nfault == 0;
}

/**
 * @brief Checks if has a fault occured on STEPPER_Z_MOTOR
 * 
 * @return Whether a fault occured
 */
bool stepper_z_has_fault()
{
    uint8_t nfault = gpio_read_input(p_stepper_motor_z->nfault_port, p_stepper_motor_z->nfault_pin);

    // Return true (fault) if nfault (active-low) is 0
    return nfault == 0;
}

/**
 * @brief Pause all motors and their ISR clocks, can resume from this point later
 */
void stepper_pause_all_motors()
{
    clock_stop_timer(STEPPER_X_TIMER);
    clock_stop_timer(STEPPER_Y_TIMER);
    clock_stop_timer(STEPPER_Z_TIMER);
    stepper_disable_all_motors();
}

/**
 * @brief Resume all motors and their ISR clocks from a previously paused state
 */
void stepper_resume_all_motors()
{
    stepper_enable_all_motors();
    clock_start_timer(STEPPER_X_TIMER);
    clock_start_timer(STEPPER_Y_TIMER);
    clock_start_timer(STEPPER_Z_TIMER);
}

/**
 * @brief Returns the current position of the stepper in mm
 * 
 * @param p_stepper_motor The stepper motor to enable
 * @return Current position in mm
 */
static int16_t stepper_get_current_pos_mm(stepper_motors_t *p_stepper_motor)
{
    return p_stepper_motor->current_pos / TRANSITIONS_PER_MM;
}

/**
 * @brief Determines the velocity values needed for smooth motion profiling across all motors
 * 
 * @param v_x Desired x-axis velocity
 * @param v_y Desired y-axis velocity
 * @param v_z Desired z-axis velocity
 */
static void stepper_update_velocities(uint16_t v_x, uint16_t v_y, uint16_t v_z, uint16_t max_a_x, uint16_t max_a_y, uint16_t max_a_z)
{
    // X-axis determine the points where the speeds need to change
    if (STEPPER_X_MAX_V * STEPPER_X_MAX_V / STEPPER_X_MAX_A > p_stepper_motor_x->transitions_to_desired_pos)
    {
        // Non-trapezoidal motion profile (change halfway)
        p_stepper_motor_x->x_1 = p_stepper_motor_x->transitions_to_desired_pos / 2;
        p_stepper_motor_x->x_2 = p_stepper_motor_x->transitions_to_desired_pos / 2;
    }
    else
    {
        // Trapezoidal motion profile
        p_stepper_motor_x->x_2 = STEPPER_X_MAX_V * STEPPER_X_MAX_V / (2 * STEPPER_X_MAX_A);
        p_stepper_motor_x->x_1 = p_stepper_motor_x->transitions_to_desired_pos - STEPPER_X_MAX_V * STEPPER_X_MAX_V / (2 * STEPPER_X_MAX_A);
    }

    // Y-axis determine the points where the speeds need to change
    if (STEPPER_Y_MAX_V * STEPPER_Y_MAX_V / STEPPER_Y_MAX_A > p_stepper_motor_y->transitions_to_desired_pos)
    {
        // Non-trapezoidal motion profile (change halfway)
        p_stepper_motor_y->x_1 = p_stepper_motor_y->transitions_to_desired_pos / 2;
        p_stepper_motor_y->x_2 = p_stepper_motor_y->transitions_to_desired_pos / 2;
    }
    else
    {
        // Trapezoidal motion profile
        p_stepper_motor_y->x_2 = STEPPER_Y_MAX_V * STEPPER_Y_MAX_V / (2 * STEPPER_Y_MAX_A);
        p_stepper_motor_y->x_1 = p_stepper_motor_y->transitions_to_desired_pos - STEPPER_Y_MAX_V * STEPPER_Y_MAX_V / (2 * STEPPER_Y_MAX_A);
    }

    // Z-axis determine the points where the speeds need to change
    if (STEPPER_Z_MAX_V * STEPPER_Z_MAX_V / STEPPER_Z_MAX_A > p_stepper_motor_z->transitions_to_desired_pos)
    {
        // Non-trapezoidal motion profile (change halfway)
        p_stepper_motor_z->x_1 = p_stepper_motor_z->transitions_to_desired_pos / 2;
        p_stepper_motor_z->x_2 = p_stepper_motor_z->transitions_to_desired_pos / 2;
    }
    else
    {
        // Trapezoidal motion profile
        p_stepper_motor_z->x_2 = STEPPER_Z_MAX_V * STEPPER_Z_MAX_V / (2 * STEPPER_Z_MAX_A);
        p_stepper_motor_z->x_1 = p_stepper_motor_z->transitions_to_desired_pos - STEPPER_Z_MAX_V * STEPPER_Z_MAX_V / (2 * STEPPER_Z_MAX_A);
    }

    // X-axis load velocities values into its timer
    if (v_x != 0)
    {
        uint16_t v_x_bounded = utils_bound(v_x, STEPPER_MIN_SPEED, STEPPER_MAX_SPEED);
        uint32_t stepper_x_initial_period = stepper_velocity_to_timer_period(v_x_bounded);
        
        // Set the acceleration
        p_stepper_motor_x->max_accel = max_a_x;
        
        // Start the timer
        clock_set_timer_period(STEPPER_X_TIMER, stepper_x_initial_period);
        clock_start_timer(STEPPER_X_TIMER);
    }

    // Y-axis load velocities values into its timer
    if (v_y != 0)
    {
        uint16_t v_y_bounded = utils_bound(v_y, STEPPER_MIN_SPEED, STEPPER_MAX_SPEED);
        uint32_t stepper_y_initial_period = stepper_velocity_to_timer_period(v_y_bounded);
        
        // Set the acceleration
        p_stepper_motor_y->max_accel = max_a_y;
        
        // Start the timer
        clock_set_timer_period(STEPPER_Y_TIMER, stepper_y_initial_period);
        clock_start_timer(STEPPER_Y_TIMER);
    }

    // Z-axis load velocities values into its timer
    if (v_z != 0)
    {
        uint16_t v_z_bounded = utils_bound(v_z, STEPPER_MIN_SPEED, STEPPER_MAX_SPEED);
        uint32_t stepper_z_initial_period = stepper_velocity_to_timer_period(v_z_bounded);
        
        // Set the acceleration
        p_stepper_motor_z->max_accel = max_a_z;
        
        // Start the timer
        clock_set_timer_period(STEPPER_Z_TIMER, stepper_z_initial_period);
        clock_start_timer(STEPPER_Z_TIMER);
    }
}

/* Command Functions */

/**
 * @brief Builds a stepper relative movement command (moves relative to current position)
 *
 * @param rel_x Relative distance to travel in X direction (mm)
 * @param rel_y Relative distance to travel in Y direction (mm)
 * @param rel_z Relative distance to travel in Z direction (mm)
 * @param vel_x Travel velocity for X movement (mm/s)
 * @param vel_y Travel velocity for Y movement (mm/s)
 * @param vel_z Travel velocity for Z movement (mm/s)
 * @return Pointer to the command object
 */
stepper_rel_command_t* stepper_build_rel_command(int16_t rel_x, int16_t rel_y, int16_t rel_z, uint16_t v_x, uint16_t v_y, uint16_t v_z)
{
    // The thing to return
    stepper_rel_command_t* p_command = (stepper_rel_command_t*) malloc(sizeof(stepper_rel_command_t));

    // Functions
    p_command->command.p_entry   = &stepper_rel_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->rel_x = rel_x;
    p_command->rel_y = rel_y;
    p_command->rel_z = rel_z;
    p_command->v_x = v_x;
    p_command->v_y = v_y;
    p_command->v_z = v_z;

    return p_command;
}

/**
 * @brief Builds a stepper chess movement command for {X,Y} (moves to absolute position)
 *
 * @param file The board column to travel to
 * @param rank The board row to travel to
 * @param vel_x Travel velocity for X movement (mm/s)
 * @param vel_y Travel velocity for Y movement (mm/s)
 * @return Pointer to the command object
 */
stepper_chess_command_t* stepper_build_chess_xy_command(chess_file_t file, chess_rank_t rank, uint16_t v_x, uint16_t v_y)
{
    // The thing to return
    stepper_chess_command_t* p_command = (stepper_chess_command_t*) malloc(sizeof(stepper_chess_command_t));

    // Functions
    p_command->command.p_entry   = &stepper_chess_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->file  = file;
    p_command->rank  = rank;
    p_command->piece = PIECE_ERROR;
    p_command->v_x   = v_x;
    p_command->v_y   = v_y;
    p_command->v_z   = 0;

    return p_command;
}

/**
 * @brief Builds a stepper chess movement command for {Z} (moves to absolute position)
 *
 * @param piece The piece type at the given tile
 * @param vel_z Travel velocity for Z movement (mm/s)
 * @return Pointer to the command object
 */
stepper_chess_command_t* stepper_build_chess_z_command(chess_piece_t piece, uint16_t v_z)
{
    // The thing to return
    stepper_chess_command_t* p_command = (stepper_chess_command_t*) malloc(sizeof(stepper_chess_command_t));

    // Functions
    p_command->command.p_entry   = &stepper_chess_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->file  = FILE_ERROR;
    p_command->rank  = RANK_ERROR;
    p_command->piece = piece;
    p_command->v_x   = 0;
    p_command->v_y   = 0;
    p_command->v_z   = v_z;

    return p_command;
}

/**
 * @brief Builds a stepper home movement command for the {X,Y} directions (separate from Z so we do not break the rack on a buttress)
 *
 * @return Pointer to the command object
 */
stepper_rel_command_t* stepper_build_home_xy_command()
{
    // The thing to return
    stepper_rel_command_t* p_command = (stepper_rel_command_t*) malloc(sizeof(stepper_rel_command_t));

    // Functions
    p_command->command.p_entry   = &stepper_home_entry;
    p_command->command.p_action  = &stepper_home_action;
    p_command->command.p_exit    = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->rel_x = STEPPER_HOME_DISTANCE;
    p_command->rel_y = -STEPPER_HOME_DISTANCE;
    p_command->rel_z = 0;
    p_command->v_x   = STEPPER_HOME_VELOCITY;
    p_command->v_y   = STEPPER_HOME_VELOCITY;
    p_command->v_z   = 0;

    return p_command;
}

/**
 * @brief Builds a stepper home movement command for the Z direction (separate from {X,Y} so we do not break the rack on a buttress)
 *
 * @return Pointer to the command object
 */
stepper_rel_command_t* stepper_build_home_z_command()
{
    // The thing to return
    stepper_rel_command_t* p_command = (stepper_rel_command_t*) malloc(sizeof(stepper_rel_command_t));

    // Functions
    p_command->command.p_entry   = &stepper_home_entry;
    p_command->command.p_action  = &stepper_home_action;
    p_command->command.p_exit    = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->rel_x = 0;
    p_command->rel_y = 0;
    p_command->rel_z = -STEPPER_HOME_DISTANCE;
    p_command->v_x   = 0;
    p_command->v_y   = 0;
    p_command->v_z   = STEPPER_HOME_VELOCITY;

    return p_command;
}

/**
 * @brief Prepares the steppers for this command
 * 
 * @param command 
 */
void stepper_rel_entry(command_t* command)
{
    stepper_rel_command_t* p_stepper_command = (stepper_rel_command_t*) command;

    // X-axis
    if (p_stepper_command->rel_x != 0)
    {
        stepper_enable_motor(p_stepper_motor_x);

        // Set the directions
        if (p_stepper_command->rel_x > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_x);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_x);
        }
    }

    // Y-axis
    if (p_stepper_command->rel_y != 0)
    {
        stepper_enable_motor(p_stepper_motor_y);

        // Set the direction
        if (p_stepper_command->rel_y > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_y);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_y);
        }
    }

    // Z-axis
    if (p_stepper_command->rel_z != 0)
    {
        stepper_enable_motor(p_stepper_motor_z);

        // Set the direction
        if (p_stepper_command->rel_z > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_z);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_z);
        }
    }

    // Determine the distances to go
    p_stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_x);
    p_stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_y);
    p_stepper_motor_z->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_z);

    // Update the velocities
    stepper_update_velocities(p_stepper_command->v_x, p_stepper_command->v_y, p_stepper_command->v_z, STEPPER_X_MAX_A, STEPPER_Y_MAX_A, STEPPER_Z_MAX_A);
}

/**
 * @brief Prepares the steppers for this command
 *
 * @param command
 */
void stepper_chess_entry(command_t* command)
{
    int16_t rel_move_x;
    int16_t rel_move_y;
    int16_t rel_move_z;
    int16_t current_x = stepper_get_current_pos_mm(p_stepper_motor_x);
    int16_t current_y = stepper_get_current_pos_mm(p_stepper_motor_y);
    int16_t current_z = stepper_get_current_pos_mm(p_stepper_motor_z);

    stepper_chess_command_t* p_stepper_command = (stepper_chess_command_t*) command;

    // X-axis
    if (p_stepper_command->file != FILE_ERROR)
    {
        stepper_enable_motor(p_stepper_motor_x);

        // Find how far we need to go to get there
        rel_move_x = p_stepper_command->file - current_x;

        // Set the directions
        if (rel_move_x > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_x);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_x);
        }
    }

    // Y-axis
    if (p_stepper_command->rank != RANK_ERROR)
    {
        stepper_enable_motor(p_stepper_motor_y);

        // Find how far we need to go to get there
        rel_move_y = p_stepper_command->rank - current_y;

        // Set the direction
        if (rel_move_y > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_y);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_y);
        }
    }

    // Z-axis
    if (p_stepper_command->piece != PIECE_ERROR)
    {
        stepper_enable_motor(p_stepper_motor_z);

        // Find how far we need to go to get there
        rel_move_z = p_stepper_command->piece - current_z;

        // Set the direction
        if (rel_move_z > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_z);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_z);
        }
    }

    // Determine the distances to go
    p_stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(rel_move_x);
    p_stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(rel_move_y);
    p_stepper_motor_z->transitions_to_desired_pos = stepper_distance_to_transitions(rel_move_z);

    // Update the velocities
    stepper_update_velocities(p_stepper_command->v_x, p_stepper_command->v_y, p_stepper_command->v_z, STEPPER_X_MAX_A, STEPPER_Y_MAX_A, STEPPER_Z_MAX_A);
}

/**
 * @brief Prepares the steppers for this command
 *
 * @param command The stepper command being run
 */
void stepper_home_entry(command_t* command)
{
    stepper_rel_command_t* p_stepper_command = (stepper_rel_command_t*) command;

    // X-axis
    if (p_stepper_command->rel_x != 0)
    {
        stepper_enable_motor(p_stepper_motor_x);

        // Set the directions
        if (p_stepper_command->rel_x > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_x);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_x);
        }
    }

    // Y-axis
    if (p_stepper_command->rel_y != 0)
    {
        stepper_enable_motor(p_stepper_motor_y);

        // Set the direction
        if (p_stepper_command->rel_y > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_y);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_y);
        }
    }

    // Z-axis
    if (p_stepper_command->rel_z != 0)
    {
        stepper_enable_motor(p_stepper_motor_z);

        // Set the direction
        if (p_stepper_command->rel_z > 0)
        {
            stepper_set_direction_counterclockwise(p_stepper_motor_z);
        }
        else
        {
            stepper_set_direction_clockwise(p_stepper_motor_z);
        }
    }

    // Determine the distances to go
    p_stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_x);
    p_stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_y);
    p_stepper_motor_z->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_z);

    // Update the velocities (max acceleration of zero prevents the speed from changing)
    stepper_update_velocities(p_stepper_command->v_x, p_stepper_command->v_y, p_stepper_command->v_z, 0, 0, 0);
}

/**
 * @brief Makes sure no limit switch has been pressed
 *
 * @param command The stepper command being run
 */
void stepper_home_action(command_t* command)
{
    // Check the current switch readings
    uint8_t switch_data = switch_get_reading();

    // If a limit switch was pressed, disable the appropriate motor
    if (switch_data & LIMIT_X_MASK)
    {
        stepper_x_stop();
        p_stepper_motor_x->current_pos = 0;
    }
    if (switch_data & LIMIT_Y_MASK)
    {
        stepper_y_stop();
        p_stepper_motor_y->current_pos = 0;
    }
    if (switch_data & LIMIT_Z_MASK)
    {
        stepper_z_stop();
        p_stepper_motor_z->current_pos = 0;
    }
}

/**
 * @brief Disables all motors once a stepper command has finished
 * 
 * @param command The stepper command exiting
 */
void stepper_exit(command_t* command)
{
    stepper_disable_all_motors();
    clock_stop_timer(STEPPER_X_TIMER);
    clock_stop_timer(STEPPER_Y_TIMER);
    clock_stop_timer(STEPPER_Z_TIMER);
}

/**
 * @brief Marks the command as done once all steppers reach their desired position
 * 
 * @param command The stepper command being evaluated
 * @return Whether all steppers have reached their desired positions
 */
bool stepper_is_done(command_t* command)
{
    bool arrived = true;
    
    // Loop through all motors
    int i = 0;
    for (i = 0; (i < NUMBER_OF_STEPPER_MOTORS) && (arrived); i++)
    {
        arrived &= (stepper_motors[i].transitions_to_desired_pos == 0);
    }

    return arrived;
}

/* Interrupts */

/**
 * @brief Helper function to calculate how the period needs to change to match the desired acceleration
 * 
 * @param p_stepper_motor The stepper motor for which period shift is being calculated
 * @return How much the period needs to change to meet the desired acceleration
 */
static uint64_t stepper_get_period_shift(stepper_motors_t* p_stepper_motor)
{
    // This computes the [at] term from the kinematic equation [v_f = v_i + at]. The coefficient [80/9] was determined by trial-and-error
//    return 80*(uint64_t)(p_stepper_motor->max_accel * clock_get_timer_period(p_stepper_motor->timer)) / (9*SYSCLOCK_FREQUENCY);
    return 0;
}

/**
 * @brief Helper function to perform the interrupt activity for a specified stepper motor
 *
 * @param p_stepper_motor The stepper motor corresponding to this interrupt
 */
static void stepper_interrupt_activity(stepper_motors_t* p_stepper_motor)
{
    // Move each stepper until it reaches its destination, then disable
    if (p_stepper_motor->transitions_to_desired_pos > 0)
    {
        // Move the motor, update the counter and position
        stepper_edge_transition(p_stepper_motor);
        p_stepper_motor->transitions_to_desired_pos -= 1;
        p_stepper_motor->current_pos += p_stepper_motor->dir;

#ifdef STEPPER_DEBUG
        // Send the data to the laptop
        char data[32];
        sprintf(data, "(%d,%d,%d)", p_stepper_motor->current_pos / TRANSITIONS_PER_MM, clock_get_timer_period(p_stepper_motor->timer), p_stepper_motor->time_elapsed); // current_pos, the number in the register, time_elapsed
        uart_out_string(PROFILING_CHANNEL, data, 32);
        // Delay so this is not spamable (we only transmit strings for testing, so this is not an issue for the actual robot)
        utils_delay(150000);
#endif

        // Update the timer period for smooth motion profiling
        uint64_t period_shift = stepper_get_period_shift(p_stepper_motor);

        if (p_stepper_motor->transitions_to_desired_pos > p_stepper_motor->x_1)      // still accelerating
        {
            clock_set_timer_period(p_stepper_motor->timer, (clock_get_timer_period(p_stepper_motor->timer) - period_shift));
        }
        else if (p_stepper_motor->transitions_to_desired_pos < p_stepper_motor->x_2) // deaccelerating
        {
            clock_set_timer_period(p_stepper_motor->timer, (clock_get_timer_period(p_stepper_motor->timer) + period_shift));
        }

        clock_start_timer(p_stepper_motor->timer);
    }
    else
    {
        // Disable the motor
        stepper_disable_motor(p_stepper_motor);
    }
}

/**
 * @brief Interrupt handler for STEPPER_X
 */
__interrupt void STEPPER_X_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(STEPPER_X_TIMER);

    // Perform the stepper interrupt activity
    stepper_interrupt_activity(p_stepper_motor_x);
}

/**
 * @brief Interrupt handler for STEPPER_Y
 */
__interrupt void STEPPER_Y_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(STEPPER_Y_TIMER);

    // Perform the stepper interrupt activity
    stepper_interrupt_activity(p_stepper_motor_y);
}

/**
 * @brief Interrupt handler for STEPPER_Z
 */
__interrupt void STEPPER_Z_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(STEPPER_Z_TIMER);

    // Perform the stepper interrupt activity
    stepper_interrupt_activity(p_stepper_motor_z);
}

/* End steppermotors.c */
