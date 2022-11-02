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

// Private functions
static void stepper_edge_transition(stepper_motors_t *stepper_motor);
static void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor);
static void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor);
static void stepper_disable_motor(stepper_motors_t *stepper_motor);
static void stepper_enable_motor(stepper_motors_t *stepper_motor);

// Declare the stepper motors
stepper_motors_t stepper_motors[NUMBER_OF_STEPPER_MOTORS];
static stepper_motors_t* stepper_motor_x = &stepper_motors[STEPPER_X_ID];
static stepper_motors_t* stepper_motor_y = &stepper_motors[STEPPER_Y_ID];
static stepper_motors_t* stepper_motor_z = &stepper_motors[STEPPER_Z_ID];

/**
 * @brief Initialize all stepper motors
 */
void stepper_init_motors()
{
    /* Stepper 1 (x direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_X_ENABLE_PORT, STEPPER_X_ENABLE_PIN);
    gpio_set_output_high(STEPPER_X_ENABLE_PORT, STEPPER_X_ENABLE_PIN);

    // MS3 | MS2 | MS1
    //   0 |   0 |  0    <=> Full step
    //   0 |   0 |  1    <=> 1/2 step
    //   0 |   1 |  0    <=> 1/4 step
    //   0 |   1 |  1    <=> 1/8 step
    //   1 |   0 |  0    <=> 1/16 step
    //   All Others      <=> 1/32 step
    gpio_set_as_output(STEPPER_X_MS1_PORT, STEPPER_X_MS1_PIN);
    gpio_set_as_output(STEPPER_X_MS2_PORT, STEPPER_X_MS2_PIN);
    gpio_set_as_output(STEPPER_X_MS3_PORT, STEPPER_X_MS3_PIN);
    gpio_set_output_high(STEPPER_X_MS1_PORT, STEPPER_X_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Counterclockwise
    gpio_set_as_output(STEPPER_X_DIR_PORT, STEPPER_X_DIR_PIN);
    gpio_set_output_high(STEPPER_X_DIR_PORT, STEPPER_X_DIR_PIN);

    // STEP is toggled to perform the stepping. Two toggles <=> one microstep
    gpio_set_as_output(STEPPER_X_STEP_PORT, STEPPER_X_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_x->dir_port                   = STEPPER_X_DIR_PORT;
    stepper_motor_x->dir_pin                    = STEPPER_X_DIR_PIN;
    stepper_motor_x->step_port                  = STEPPER_X_STEP_PORT;
    stepper_motor_x->step_pin                   = STEPPER_X_STEP_PIN;
    stepper_motor_x->enable_port                = STEPPER_X_ENABLE_PORT;
    stepper_motor_x->enable_pin                 = STEPPER_X_ENABLE_PIN;
    stepper_motor_x->current_state              = disabled;
    stepper_motor_x->transitions_to_desired_pos = 0;
    stepper_motor_x->dir                        = 1;
    stepper_motor_x->current_pos                = 0;
    stepper_motor_x->current_vel                = 0;


    /* Stepper 2 (y direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_Y_ENABLE_PORT, STEPPER_Y_ENABLE_PIN);
    gpio_set_output_high(STEPPER_Y_ENABLE_PORT, STEPPER_Y_ENABLE_PIN);

    // MS1 | MS2 | MS3
    //   0 |   0 |  0    <=> Full step
    //   0 |   0 |  1    <=> 1/2 step
    //   0 |   1 |  0    <=> 1/4 step
    //   0 |   1 |  1    <=> 1/8 step
    //   1 |   0 |  0    <=> 1/16 step
    //   All Others      <=> 1/32 step
    gpio_set_as_output(STEPPER_Y_MS1_PORT, STEPPER_Y_MS1_PIN);
    gpio_set_as_output(STEPPER_Y_MS2_PORT, STEPPER_Y_MS2_PIN);
    gpio_set_as_output(STEPPER_Y_MS3_PORT, STEPPER_Y_MS3_PIN);
    gpio_set_output_high(STEPPER_Y_MS1_PORT, STEPPER_Y_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Counterclockwise
    gpio_set_as_output(STEPPER_Y_DIR_PORT, STEPPER_Y_DIR_PIN);
    gpio_set_output_high(STEPPER_Y_DIR_PORT, STEPPER_Y_DIR_PIN);

    // STEP is toggled to perform the stepping. Two toggles <=> one microstep
    gpio_set_as_output(STEPPER_Y_STEP_PORT, STEPPER_Y_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_y->dir_port                   = STEPPER_Y_DIR_PORT;
    stepper_motor_y->dir_pin                    = STEPPER_Y_DIR_PIN;
    stepper_motor_y->step_port                  = STEPPER_Y_STEP_PORT;
    stepper_motor_y->step_pin                   = STEPPER_Y_STEP_PIN;
    stepper_motor_y->enable_port                = STEPPER_Y_ENABLE_PORT;
    stepper_motor_y->enable_pin                 = STEPPER_Y_ENABLE_PIN;
    stepper_motor_y->current_state              = disabled;
    stepper_motor_y->transitions_to_desired_pos = 0;
    stepper_motor_y->dir                        = 1;
    stepper_motor_y->current_pos                = 0;
    stepper_motor_y->current_vel                = 0;


    /* Stepper 3 (a direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_Z_ENABLE_PORT, STEPPER_Z_ENABLE_PIN);
    gpio_set_output_high(STEPPER_Z_ENABLE_PORT, STEPPER_Z_ENABLE_PIN);

    // MS1 | MS2 | MS3
    //   0 |   0 |  0    <=> Full step
    //   0 |   0 |  1    <=> 1/2 step
    //   0 |   1 |  0    <=> 1/4 step
    //   0 |   1 |  1    <=> 1/8 step
    //   1 |   0 |  0    <=> 1/16 step
    //   All Others      <=> 1/32 step
    gpio_set_as_output(STEPPER_Z_MS1_PORT, STEPPER_Z_MS1_PIN);
    gpio_set_as_output(STEPPER_Z_MS2_PORT, STEPPER_Z_MS2_PIN);
    gpio_set_as_output(STEPPER_Z_MS3_PORT, STEPPER_Z_MS3_PIN);
    gpio_set_output_high(STEPPER_Z_MS1_PORT, STEPPER_Z_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Counterclockwise
    gpio_set_as_output(STEPPER_Z_DIR_PORT, STEPPER_Z_DIR_PIN);
    gpio_set_output_high(STEPPER_Z_DIR_PORT, STEPPER_Z_DIR_PIN);

    // STEP is toggled to perform the stepping. Two toggles <=> one microstep
    gpio_set_as_output(STEPPER_Z_STEP_PORT, STEPPER_Z_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_z->dir_port                   = STEPPER_Z_DIR_PORT;
    stepper_motor_z->dir_pin                    = STEPPER_Z_DIR_PIN;
    stepper_motor_z->step_port                  = STEPPER_Z_STEP_PORT;
    stepper_motor_z->step_pin                   = STEPPER_Z_STEP_PIN;
    stepper_motor_z->enable_port                = STEPPER_Z_ENABLE_PORT;
    stepper_motor_z->enable_pin                 = STEPPER_Z_ENABLE_PIN;
    stepper_motor_z->current_state              = disabled;
    stepper_motor_z->transitions_to_desired_pos = 0;
    stepper_motor_z->dir                        = 1;
    stepper_motor_z->current_pos                = 0;
    stepper_motor_z->current_vel                = 0;
}

/**
 * @brief Change the direction of the stepper motor identified by StepperID to clockwise
 * 
 * @param stepper_motor The stepper motor to set direction on 
 */
void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_high(stepper_motor->dir_port, stepper_motor->dir_pin);
    stepper_motor->dir = -1;
}

/**
 * @brief Change the direction of the stepper motor identified by StepperID to counterclockwise
 * 
 * @param The stepper motor to toggle direction on
 */
static void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_low(stepper_motor->dir_port, stepper_motor->dir_pin);
    stepper_motor->dir = 1;
}

/**
 * @brief Perform an edge transition on the stepper STEP pin
 * 
 * @param The stepper motor to perform the transition on
 */
static void stepper_edge_transition(stepper_motors_t *stepper_motor)
{
    gpio_set_output_toggle(stepper_motor->step_port, stepper_motor->step_pin);
}

/**
 * @brief Convert a distance (mm) to the number of edge transitions required to drive a stepper that far
 *  Stepper performs 200 steps/revolution. We microstep to 1/2 steps, so 400 microsteps/revolution
 *  Belt pitch is 2 mm, and rotor has 20 teeth, so 40mm/revolution
 *  There are 2 transitions/microstep
 *      ==> (2 transitions/microstep)*(400 microsteps/revolution)/(40mm/revolution) = 20 transitions/mm
 * 
 * @param distance The distance to travel
 * @return The number of edge transitions to travel that distance
 */
static uint32_t stepper_distance_to_transitions(int32_t distance)
{
    if (distance < 0)
    {
        return (-20)*distance;
    }
    return 20*distance;
}

/**
 * @brief Disable the specified stepper and update its current_state
 * 
 * @param The stepper motor to disable
 */
static void stepper_disable_motor(stepper_motors_t *stepper_motor)
{
    gpio_set_output_high(stepper_motor->enable_port, stepper_motor->enable_pin);
    stepper_motor->current_state = disabled;
}

/**
 * @brief Disable all motors
 */
void stepper_disable_all_motors()
{
    stepper_motors_t* base_stepper = &stepper_motors[0];
    uint8_t i = 0;

    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_disable_motor((base_stepper + i));
    }
}

/**
 * @brief Enable the specified stepper and update its current_state
 * 
 * @param The stepper motor to enable
 */
static void stepper_enable_motor(stepper_motors_t *stepper_motor)
{
    gpio_set_output_low(stepper_motor->enable_port, stepper_motor->enable_pin);
    stepper_motor->current_state = enabled;
}

/**
 * @brief Enable all motors
 */
void stepper_enable_all_motors()
{
    stepper_motors_t* base_stepper = &stepper_motors[0];
    uint8_t i = 0;
    
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_enable_motor((base_stepper + i));
    }
}

/**
 * @brief Disables the STEPPER_X motor
 * 
 */
void stepper_x_stop()
{
    stepper_disable_motor(stepper_motor_x);
    stepper_motor_x->transitions_to_desired_pos = 0;
    clock_pause_timer(STEPPER_X_TIMER);
}

/**
 * @brief Disables the STEPPER_Y motor
 * 
 */
void stepper_y_stop()
{
    stepper_disable_motor(stepper_motor_y);
    stepper_motor_y->transitions_to_desired_pos = 0;
    clock_pause_timer(STEPPER_Y_TIMER);
}

/**
 * @brief Disables the STEPPER_Z motor
 * 
 */
void stepper_z_stop()
{
    stepper_disable_motor(stepper_motor_z);
    stepper_motor_z->transitions_to_desired_pos = 0;
    clock_pause_timer(STEPPER_Z_TIMER);
}

/**
 * @brief Stop the motors and the ISR clock, can resume from this point later
 */
void stepper_pause_motors()
{
    clock_pause_timer(STEPPER_X_TIMER);
    stepper_disable_all_motors();
}

/**
 * @brief Resume the motors and the ISR clock from its paused state
 * 
 */
void stepper_resume_motors()
{
    clock_resume_timer(STEPPER_X_TIMER);
    stepper_enable_all_motors();
}

/**
 * @brief Returns the current position of the stepper in mm
 *
 */
int16_t stepper_x_get_current_pos()
{
    return stepper_motor_x->current_pos / 20; // TODO: Make this calculated from the stepping size
}

int16_t stepper_y_get_current_pos()
{
    return stepper_motor_y->current_pos / 20;
}

int16_t stepper_z_get_current_pos()
{
    return stepper_motor_z->current_pos / 20;
}


/* Command Functions */

stepper_rel_command_t* stepper_build_rel_command(int16_t rel_x, int16_t rel_y, int16_t rel_z, uint16_t v_x, uint16_t v_y, uint16_t v_z)
{
    // The thing to return
    stepper_rel_command_t* p_command = (stepper_rel_command_t*)malloc(sizeof(stepper_rel_command_t));

    // Functions
    p_command->command.p_entry = &stepper_rel_entry;
    p_command->command.p_action = &stepper_action;
    p_command->command.p_exit = &stepper_exit;
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

stepper_chess_command_t* stepper_build_chess_command(chess_file_t file, chess_rank_t rank, uint16_t v_x, uint16_t v_y, uint16_t v_z)
{
    // The thing to return
    stepper_chess_command_t* p_command = (stepper_chess_command_t*)malloc(sizeof(stepper_chess_command_t));

    // Functions
    p_command->command.p_entry = &stepper_chess_entry;
    p_command->command.p_action = &stepper_action;
    p_command->command.p_exit = &stepper_exit;
    p_command->command.p_is_done = &stepper_is_done;

    // Data
    p_command->file = file;
    p_command->rank = rank;
    p_command->v_x = v_x;
    p_command->v_y = v_y;

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
        stepper_enable_motor(stepper_motor_x);

        // Set the directions
        if (p_stepper_command->rel_x > 0)
        {
            stepper_set_direction_counterclockwise(stepper_motor_x);
        }
        else
        {
            stepper_set_direction_clockwise(stepper_motor_x);
        }
    }

    // Y-axis
    if (p_stepper_command->rel_y != 0)
    {
        stepper_enable_motor(stepper_motor_y);

        // Set the direction
        if (p_stepper_command->rel_y > 0)
        {
            stepper_set_direction_counterclockwise(stepper_motor_y);
        }
        else
        {
            stepper_set_direction_clockwise(stepper_motor_y);
        }
    }

    // Z-axis
    if (p_stepper_command->rel_z != 0)
    {
        stepper_enable_motor(stepper_motor_z);

        // Set the direction
        if (p_stepper_command->rel_z > 0)
        {
            stepper_set_direction_counterclockwise(stepper_motor_z);
        }
        else
        {
            stepper_set_direction_clockwise(stepper_motor_z);
        }
    }

    // Determine the distance to go
    stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_x);
    stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_y);
    stepper_motor_z->transitions_to_desired_pos = stepper_distance_to_transitions(p_stepper_command->rel_z);

    // TODO: Load velocity values into the clock
    if (p_stepper_command->v_x != 0)
    {
        uint16_t v_x = utils_bound(p_stepper_command->v_x, STEPPER_MIN_SPEED, STEPPER_MAX_SPEED);
        uint32_t stepper_x_period = 120000000 / (stepper_distance_to_transitions(v_x));
        clock_set_timer_period(STEPPER_X_TIMER, stepper_x_period); // Currently only X has an interrupt
        clock_resume_timer(STEPPER_X_TIMER);
    }
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
    int16_t current_x = stepper_x_get_current_pos();
    int16_t current_y = stepper_y_get_current_pos();

    stepper_chess_command_t* p_stepper_command = (stepper_chess_command_t*) command;

    // X-axis
    if (p_stepper_command->file != FILE_ERROR)
    {
        stepper_enable_motor(stepper_motor_x);

        // Find how far we need to go to get there
        rel_move_x = p_stepper_command->file - stepper_x_get_current_pos();

        // Set the directions
        if (rel_move_x > 0)
        {
            stepper_set_direction_counterclockwise(stepper_motor_x);
        }
        else
        {
            stepper_set_direction_clockwise(stepper_motor_x);
        }
    }

    // Y-axis
    if (p_stepper_command->rank != RANK_ERROR)
    {
        stepper_enable_motor(stepper_motor_y);

        // Find how far we need to go to get there
        rel_move_y = p_stepper_command->rank - stepper_y_get_current_pos();

        // Set the direction
        if (rel_move_y > 0)
        {
            stepper_set_direction_counterclockwise(stepper_motor_y);
        }
        else
        {
            stepper_set_direction_clockwise(stepper_motor_y);
        }
    }

    // Determine the distance to go
    stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(rel_move_x);
    stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(rel_move_y);

    // TODO: Load velocity values into the clock
    if (p_stepper_command->v_x != 0)
    {
        uint16_t v_x = utils_bound(p_stepper_command->v_x, STEPPER_MIN_SPEED, STEPPER_MAX_SPEED);
        uint32_t stepper_x_period = 120000000 / (stepper_distance_to_transitions(v_x));
        clock_set_timer_period(STEPPER_X_TIMER, stepper_x_period); // Currently only X has an interrupt
        clock_resume_timer(STEPPER_X_TIMER);
    }
}

/**
 * @brief Stepping is handled in the ISR, so this action is empty
 * 
 * @param command The stepper command being run
 */
void stepper_action(command_t* command)
{
    return;
}

/**
 * @brief Disables all motors once a stepper command has finished
 * 
 * @param command The stepper command exiting
 */
void stepper_exit(command_t* command)
{
    stepper_disable_all_motors();
    clock_pause_timer(STEPPER_X_TIMER);
    clock_pause_timer(STEPPER_Y_TIMER);
    clock_pause_timer(STEPPER_Z_TIMER);
}

/**
 * @brief Marks the command as done once all steppers reach their desired position
 * 
 * @param command The stepper command being evaluated
 * @return true Once all steppers reach their desired positions
 * @return false If at least one stepper has not reached its desired position
 */
bool stepper_is_done(command_t* command)
{
    return (stepper_motor_x->transitions_to_desired_pos == 0) &&
           (stepper_motor_y->transitions_to_desired_pos == 0) &&
           (stepper_motor_z->transitions_to_desired_pos == 0);
}

/**
 * @brief Interrupt handler for the stepper module
 */
__interrupt void STEPPER_X_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(STEPPER_X_TIMER);

    // Move each stepper until it reaches its destination, then disable
    stepper_motors_t* base_stepper = &stepper_motors[0];
    uint8_t i = 0;
    
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        if ((base_stepper+i)->transitions_to_desired_pos > 0) 
        {
            // Move the motor
            stepper_edge_transition((base_stepper+i));
            // Update our counter
            (base_stepper+i)->transitions_to_desired_pos -= 1;
            // Update the position
            (base_stepper+i)->current_pos += (base_stepper+i)->dir;
        } 
        else
        {
            stepper_disable_motor((base_stepper+i));
        }
    }
}

/* End steppermotors.c */
