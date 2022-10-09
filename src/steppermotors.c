/**
 * @file steppermotors.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control stepper motors driven by DRV8824-Q1 chips
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#include "steppermotors.h"
#include "stepper_fifo.h"

// Assumed board layout:
//  |-------|
//  | BOARD |
//  |-------X  <== Position (0,0), home

// Declare the stepper motors
stepper_motors_t stepper_motors[NUMBER_OF_STEPPER_MOTORS];
static stepper_motors_t* stepper_motor_x = &stepper_motors[0];
static stepper_motors_t* stepper_motor_y = &stepper_motors[1];

// Declare queues for the pending motor positions
stepper_fifo_t stepper_pos_queues[NUMBER_OF_STEPPER_MOTORS];
static stepper_fifo_t* stepper_x_pos_queue = &stepper_pos_queues[0];
static stepper_fifo_t* stepper_y_pos_queue = &stepper_pos_queues[1];

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

    // Configure the position queue
    stepper_fifo_init(stepper_x_pos_queue);


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
    stepper_fifo_init(stepper_y_pos_queue);
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
 * @param The stepper motor to perform the transition on
 */
static void stepper_disable_motor(stepper_motors_t *stepper_motor)
{
    gpio_set_output_high(stepper_motor->enable_port, stepper_motor->enable_pin);
    stepper_motor->current_state = disabled;
}

/**
 * @brief Enable the specified stepper and update its current_state
 * 
 * @param The stepper motor to perform the transition on
 */
static void stepper_enable_motor(stepper_motors_t *stepper_motor)
{
    gpio_set_output_low(stepper_motor->enable_port, stepper_motor->enable_pin);
    stepper_motor->current_state = enabled;
}

static void stepper_get_next_pos_commands()
{
    // Disable the motors
    stepper_disable_motor(stepper_motor_x);
    stepper_disable_motor(stepper_motor_y);

    // Retrieve the next position commands
    int32_t distance_x = 0;
    int32_t distance_y = 0;
    stepper_fifo_pop(stepper_x_pos_queue, (int32_t*) &distance_x);
    stepper_fifo_pop(stepper_y_pos_queue, (int32_t*) &distance_y);

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
 */
void stepper_go_to_rel_position(int32_t distance_x, int32_t distance_y)
{
    // Load the this command onto the position queues
    stepper_fifo_push(stepper_x_pos_queue, distance_x);
    stepper_fifo_push(stepper_y_pos_queue, distance_y);

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
    // Load the this command onto the position queues
    stepper_fifo_push(stepper_x_pos_queue, stepper_motor_x->distance_to_home);
    stepper_fifo_push(stepper_y_pos_queue, stepper_motor_y->distance_to_home);

    // (EXPERIMENTAL) Update the distance to home, assuming we reach our destination
    stepper_motor_x->distance_to_home = 0;
    stepper_motor_y->distance_to_home = 0;
}

/**
 * @brief Interrupt handler to check if the desired number of steps are complete
 */
__interrupt void STEPPER_HANDLER(void)
{
    // Clear the interrupt flag
    TIMER0->ICR |= (TIMER_ICR_TATOCINT);

    // Check for a new position command
    if ((stepper_motor_x->transitions_to_desired_pos == 0) && (stepper_motor_y->transitions_to_desired_pos == 0))
    {
        stepper_get_next_pos_commands();
    }

    // Move stepper_x until it reaches its destination
    if (stepper_motor_x->transitions_to_desired_pos > 0) 
    {
        stepper_edge_transition(stepper_motor_x);
        stepper_motor_x->transitions_to_desired_pos -= 1;
    } 
    else
    {
        stepper_disable_motor(stepper_motor_x);
    }

    // Move stepper_y until it reaches its destination
    if (stepper_motor_y->transitions_to_desired_pos > 0) 
    {
        stepper_edge_transition(stepper_motor_y);
        stepper_motor_y->transitions_to_desired_pos -= 1;
    } 
    else 
    {
        stepper_disable_motor(stepper_motor_y);
    }
}

/* End steppermotors.c */
