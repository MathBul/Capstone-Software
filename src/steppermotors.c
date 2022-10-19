/**
 * @file steppermotors.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control stepper motors driven by DRV8824-Q1 chips
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#include "clock.h"
#include "stepper_fifo.h"
#include "steppermotors.h"

// Declare the stepper motors
stepper_motors_t stepper_motors[NUMBER_OF_STEPPER_MOTORS];
static stepper_motors_t* stepper_motor_x = &stepper_motors[0];
static stepper_motors_t* stepper_motor_y = &stepper_motors[1];

// Declare the position command queue
static stepper_fifo_t stepper_pos_queue;
static stepper_fifo_t* p_stepper_pos_queue = &stepper_pos_queue;

/**
 * @brief Initialize all stepper motors
 */
void stepper_init_motors()
{
    /* Stepper 1 (x direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_1_ENABLE_PORT, STEPPER_1_ENABLE_PIN);
    gpio_set_output_high(STEPPER_1_ENABLE_PORT, STEPPER_1_ENABLE_PIN);

    // MS3 | MS2 | MS1
    //   0 |   0 |  0    <=> Full step
    //   0 |   0 |  1    <=> 1/2 step
    //   0 |   1 |  0    <=> 1/4 step
    //   0 |   1 |  1    <=> 1/8 step
    //   1 |   0 |  0    <=> 1/16 step
    //   All Others      <=> 1/32 step
    gpio_set_as_output(STEPPER_1_MS1_PORT, STEPPER_1_MS1_PIN);
    gpio_set_as_output(STEPPER_1_MS2_PORT, STEPPER_1_MS2_PIN);
    gpio_set_as_output(STEPPER_1_MS3_PORT, STEPPER_1_MS3_PIN);
    gpio_set_output_high(STEPPER_1_MS1_PORT, STEPPER_1_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Counterclockwise
    gpio_set_as_output(STEPPER_1_DIR_PORT, STEPPER_1_DIR_PIN);
    gpio_set_output_high(STEPPER_1_DIR_PORT, STEPPER_1_DIR_PIN);

    // STEP is toggled to perform the stepping. Two toggles <=> one microstep
    gpio_set_as_output(STEPPER_1_STEP_PORT, STEPPER_1_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_x->dir_port                   = STEPPER_1_DIR_PORT;
    stepper_motor_x->dir_pin                    = STEPPER_1_DIR_PIN;
    stepper_motor_x->step_port                  = STEPPER_1_STEP_PORT;
    stepper_motor_x->step_pin                   = STEPPER_1_STEP_PIN;
    stepper_motor_x->enable_port                = STEPPER_1_ENABLE_PORT;
    stepper_motor_x->enable_pin                 = STEPPER_1_ENABLE_PIN;
    stepper_motor_x->current_state              = disabled;
    stepper_motor_x->distance_to_home           = 0;
    stepper_motor_x->current_position           = pos_1;
    stepper_motor_x->transitions_to_desired_pos = 0;


    /* Stepper 2 (y direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_2_ENABLE_PORT, STEPPER_2_ENABLE_PIN);
    gpio_set_output_high(STEPPER_2_ENABLE_PORT, STEPPER_2_ENABLE_PIN);

    // MS1 | MS2 | MS3
    //   0 |   0 |  0    <=> Full step
    //   0 |   0 |  1    <=> 1/2 step
    //   0 |   1 |  0    <=> 1/4 step
    //   0 |   1 |  1    <=> 1/8 step
    //   1 |   0 |  0    <=> 1/16 step
    //   All Others      <=> 1/32 step
    gpio_set_as_output(STEPPER_2_MS1_PORT, STEPPER_2_MS1_PIN);
    gpio_set_as_output(STEPPER_2_MS2_PORT, STEPPER_2_MS2_PIN);
    gpio_set_as_output(STEPPER_2_MS3_PORT, STEPPER_2_MS3_PIN);
    gpio_set_output_high(STEPPER_2_MS1_PORT, STEPPER_2_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Counterclockwise
    gpio_set_as_output(STEPPER_2_DIR_PORT, STEPPER_2_DIR_PIN);
    gpio_set_output_high(STEPPER_2_DIR_PORT, STEPPER_2_DIR_PIN);

    // STEP is toggled to perform the stepping. Two toggles <=> one microstep
    gpio_set_as_output(STEPPER_2_STEP_PORT, STEPPER_2_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_y->dir_port                   = STEPPER_2_DIR_PORT;
    stepper_motor_y->dir_pin                    = STEPPER_2_DIR_PIN;
    stepper_motor_y->step_port                  = STEPPER_2_STEP_PORT;
    stepper_motor_y->step_pin                   = STEPPER_2_STEP_PIN;
    stepper_motor_y->enable_port                = STEPPER_2_ENABLE_PORT;
    stepper_motor_y->enable_pin                 = STEPPER_2_ENABLE_PIN;
    stepper_motor_y->current_state              = disabled;
    stepper_motor_y->distance_to_home           = 0;
    stepper_motor_y->current_position           = pos_1;
    stepper_motor_y->transitions_to_desired_pos = 0;


    // Configure the position queue
    stepper_fifo_init(p_stepper_pos_queue);
}

/**
 * @brief Toggle the direction of the given stepper_motor
 * 
 * @param stepper_motor The stepper motor to toggle direction on
 */
static void stepper_toggle_direction(stepper_motors_t *stepper_motor)
{
    gpio_set_output_toggle(stepper_motor->dir_port, stepper_motor->dir_pin);
}

/**
 * @brief Change the direction of the stepper motor identified by StepperID to clockwise
 * 
 * @param stepper_motor The stepper motor to set direction on 
 */
void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_high(stepper_motor->dir_port, stepper_motor->dir_pin);
}

/**
 * @brief Change the direction of the stepper motor identified by StepperID to counterclockwise
 * 
 * @param The stepper motor to toggle direction on
 */
static void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_low(stepper_motor->dir_port, stepper_motor->dir_pin);
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
 * @brief Convert a distance in mm to the number of edge transitions required to drive the stepper that far
 *  Stepper performs 200 steps/revolution. We microstep to 1/2 steps, so 400 microsteps/revolution
 *  Belt pitch is 2 mm, and rotor has 20 teeth, so 40mm/revolution
 *  There are 2 transitions/microstep
 *      ==> (2 transitions/microstep)*(400 microsteps/revolution)/(40mm/revolution) = 20 transitions/mm
 * 
 * @param distance The distance to travel
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
void stepper_disable_motor(uint8_t stepper_id)
{
    // Assert at least one of the motors is specified?
    
    stepper_motors_t* stepper_motor;
    switch (stepper_id) 
    {
        case (0):
            stepper_motor = stepper_motor_x;
            break;
        case (1):
            stepper_motor = stepper_motor_y;
            break;
        default:
            stepper_motor = stepper_motor_x;
            break;
    }

    gpio_set_output_high(stepper_motor->enable_port, stepper_motor->enable_pin);
    stepper_motor->current_state = disabled;
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
 * @brief Disable all motors, regardless of many are added
 */
static void stepper_disable_all_motors()
{
    uint8_t i = 0;
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_disable_motor(i);
    }
}

/**
 * @brief Enable all motors, regardless of many are added
 */
static void stepper_enable_all_motors()
{
    stepper_motors_t* local_stepper = &stepper_motors[0];
    uint8_t i = 0;
    for (i = 0; i < NUMBER_OF_STEPPER_MOTORS; i++)
    {
        stepper_enable_motor((local_stepper + i));
    }
}

/**
 * @brief Pulls a position command off of the queue
 * 
 */
void stepper_get_next_pos_commands()
{
    // Disable the motors
    stepper_disable_motor(STEPPER_X_ID);
    stepper_disable_motor(STEPPER_Y_ID);

    // Retrieve the next position commands
    stepper_fifo_node_t position_command = (stepper_fifo_node_t) { 0, 0, 0 };
    stepper_fifo_pop(p_stepper_pos_queue, &position_command);
    int32_t distance_x = position_command.distance_x;
    int32_t distance_y = position_command.distance_y;

    // Determine number of steps to reach the desired positions
    stepper_motor_x->transitions_to_desired_pos = stepper_distance_to_transitions(distance_x);
    stepper_motor_y->transitions_to_desired_pos = stepper_distance_to_transitions(distance_y);

    // Set the directions
    if (distance_x > 0)
    {
        stepper_set_direction_counterclockwise(stepper_motor_x);
    }
    else 
    {
        stepper_set_direction_clockwise(stepper_motor_x);
    }

    if (distance_y > 0)
    {
        stepper_set_direction_counterclockwise(stepper_motor_y);
    }
    else 
    {
        stepper_set_direction_clockwise(stepper_motor_y);
    }    

    // Enable the motors
    stepper_enable_motor(stepper_motor_x);
    stepper_enable_motor(stepper_motor_y);
}

/**
 * @brief Method to tell the stepper motors a relative distance to travel
 * 
 * @param distance_x The distance to travel in the x-direction
 * @param distance_y The distance to travel in the y-direction
 * @param distance_y The distance to travel in the z-direction
 */
void stepper_go_to_rel_position(int32_t distance_x, int32_t distance_y, int32_t distance_z)
{
    // Prepare the position command
    stepper_fifo_node_t position_command = (stepper_fifo_node_t) { distance_x, distance_y, distance_z };

    // Load this command on the position queue
    stepper_fifo_push(p_stepper_pos_queue, position_command);

    // (EXPERIMENTAL) Update the distance to home, assuming we reach our destination
    stepper_motor_x->distance_to_home -= distance_x;
    stepper_motor_y->distance_to_home -= distance_y;
}

/**
 * @brief Method to tell the stepper motors to go home based on their relative position.
 *  Assumes all "go to" position commands were completed successfully
 */
void stepper_go_home()
{
    // Prepare the position command
    stepper_fifo_node_t position_command = (stepper_fifo_node_t) 
    { 
        stepper_motor_x->distance_to_home, 
        stepper_motor_y->distance_to_home, 
        0                                   // Todo, replace once z-axis is implemented
    };

    // Load the this command onto the position queue
    stepper_fifo_push(p_stepper_pos_queue, position_command);

    // (EXPERIMENTAL) Update the distance to home, assuming we reach our destination
    stepper_motor_x->distance_to_home = 0;
    stepper_motor_y->distance_to_home = 0;
}

/**
 * @brief Checks if all steppers have arrived at their specified destination
 * 
 * @return true If all steppers have no transitions left
 * @return false If any steppers have transitions left
 */
bool stepper_arrived()
{
    return ((stepper_motor_x->transitions_to_desired_pos == 0) && 
            (stepper_motor_y->transitions_to_desired_pos == 0));
}

/**
 * @brief Performs a transition on each motor that has not arrived at its desired position, or disables that motor
 */
void stepper_take_action()
{
    // Move stepper_x until it reaches its destination
    if (stepper_motor_x->transitions_to_desired_pos > 0) 
    {
        stepper_edge_transition(stepper_motor_x);
        stepper_motor_x->transitions_to_desired_pos -= 1;
    } 
    else
    {
        stepper_disable_motor(STEPPER_X_ID);
    }

    // Move stepper_y until it reaches its destination
    if (stepper_motor_y->transitions_to_desired_pos > 0) 
    {
        stepper_edge_transition(stepper_motor_y);
        stepper_motor_y->transitions_to_desired_pos -= 1;
    } 
    else 
    {
        stepper_disable_motor(STEPPER_Y_ID);
    }
}

/**
 * @brief Stop the motors and the ISR clock, can resume from this point later
 */
void stepper_stop_motors()
{
    clock_pause_timer(STEPPER_TIMER, timer_a);
    stepper_disable_all_motors();
}

/**
 * @brief Resume the motors and the ISR clock from its paused state
 * 
 */
void stepper_resume_motors()
{
    clock_resume_timer(STEPPER_TIMER, timer_a);
    stepper_enable_all_motors();
}

/**
 * @brief Waits until the position queue is empty
 */
static void stepper_wait()
{
    while (stepper_fifo_get_size(p_stepper_pos_queue) > 0)
    {
        // TODO: Make sure things are not added to the queue
    }
}

/**
 * @brief Reset the steppers (send home, clear command queue)
 */
void stepper_reset()
{
    // Clear the command queue
    stepper_fifo_init(p_stepper_pos_queue);

    // Return to home
    stepper_go_home();

    // Wait to get there
    stepper_wait();

    // Make sure the command queue is empty
    stepper_fifo_init(p_stepper_pos_queue);
}

/* End steppermotors.c */
