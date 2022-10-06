#include "steppermotors.h"

// Declare the stepper motors
stepper_motors_t stepper_motors[NUMBER_OF_STEPPER_MOTORS];
static stepper_motors_t* stepper_motor_x = &stepper_motors[0];
static stepper_motors_t* stepper_motor_y = &stepper_motors[1];

/*
 * Initialize a stepper motor
 */
void stepper_initialize_motors()
{
    /* Stepper 1 (x direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_1_ENABLE_PORT, STEPPER_1_ENABLE_PIN);

    // MS1 | MS2 | MS3
    //   0 |   0 |  0    <=> Full Step
    gpio_set_as_output(STEPPER_1_MS1_PORT, STEPPER_1_MS1_PIN);
    gpio_set_as_output(STEPPER_1_MS2_PORT, STEPPER_1_MS2_PIN);
    gpio_set_as_output(STEPPER_1_MS3_PORT, STEPPER_1_MS3_PIN);
    gpio_set_output_high(STEPPER_1_MS1_PORT, STEPPER_1_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Clockwise
    gpio_set_as_output(STEPPER_1_DIR_PORT, STEPPER_1_DIR_PIN);
    gpio_set_output_high(STEPPER_1_DIR_PORT, STEPPER_1_DIR_PIN);

    // STEP is toggled to perform the stepping
    gpio_set_as_output(STEPPER_1_STEP_PORT, STEPPER_1_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_x->motor_id              = STEPPER_1_ID;
    stepper_motor_x->dir_port              = STEPPER_1_DIR_PORT;
    stepper_motor_x->dir_pin               = STEPPER_1_DIR_PIN;
    stepper_motor_x->step_port             = STEPPER_1_STEP_PORT;
    stepper_motor_x->step_pin              = STEPPER_1_STEP_PIN;
    stepper_motor_x->enable_port           = STEPPER_1_ENABLE_PORT;
    stepper_motor_x->enable_pin            = STEPPER_1_ENABLE_PIN;
    stepper_motor_x->current_state         = disabled;
    stepper_motor_x->current_x_position    = col_a;
    stepper_motor_x->current_y_position    = row_1;
    stepper_motor_x->desired_x_position    = col_a;
    stepper_motor_x->desired_y_position    = row_1;
    stepper_motor_x->transitions_to_desired_x_position = 0;


    /* Stepper 2 (y direction) */
    // Enable pin is active low
    gpio_set_as_output(STEPPER_2_ENABLE_PORT, STEPPER_2_ENABLE_PIN);

    // MS1 | MS2 | MS3
    //   0 |   0 |  0    <=> Full Step
    gpio_set_as_output(STEPPER_2_MS1_PORT, STEPPER_2_MS1_PIN);
    gpio_set_as_output(STEPPER_2_MS2_PORT, STEPPER_2_MS2_PIN);
    gpio_set_as_output(STEPPER_2_MS3_PORT, STEPPER_2_MS3_PIN);
    gpio_set_output_high(STEPPER_2_MS1_PORT, STEPPER_2_MS1_PIN);

    // Set the direction: 1 <=> Clockwise; 0 <=> Clockwise
    gpio_set_as_output(STEPPER_2_DIR_PORT, STEPPER_2_DIR_PIN);
    gpio_set_output_high(STEPPER_2_DIR_PORT, STEPPER_2_DIR_PIN);

    // STEP is toggled to perform the stepping
    gpio_set_as_output(STEPPER_2_STEP_PORT, STEPPER_2_STEP_PIN);

    // Configure the stepper struct
    stepper_motor_y->motor_id              = STEPPER_2_ID;
    stepper_motor_y->dir_port              = STEPPER_2_DIR_PORT;
    stepper_motor_y->dir_pin               = STEPPER_2_DIR_PIN;
    stepper_motor_y->step_port             = STEPPER_2_STEP_PORT;
    stepper_motor_y->step_pin              = STEPPER_2_STEP_PIN;
    stepper_motor_y->enable_port           = STEPPER_2_ENABLE_PORT;
    stepper_motor_y->enable_pin            = STEPPER_2_ENABLE_PIN;
    stepper_motor_y->current_state         = disabled;
    stepper_motor_y->current_y_position    = col_a;
    stepper_motor_y->current_y_position    = row_2;
    stepper_motor_y->desired_y_position    = col_a;
    stepper_motor_y->desired_y_position    = row_2;
    stepper_motor_y->transitions_to_desired_y_position = 0;
}

/*
 * Toggle the direction of the stepper motor identified by StepperID
 */
void stepper_toggle_direction(stepper_motors_t *stepper_motor)
{
    gpio_set_output_toggle(stepper_motor->dir_port, stepper_motor->dir_pin);
}

/*
 * Change the direction of the stepper motor identified by StepperID to clockwise
 */
void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_high(stepper_motor->dir_port, stepper_motor->dir_pin);
}

/*
 * Change the direction of the stepper motor identified by StepperID to counterclockwise
 */
void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor)
{
    gpio_set_output_low(stepper_motor->dir_port, stepper_motor->dir_pin);
}

/*
 * Perform an edge transition on the stepper STEP pin
 */
void stepper_edge_transition(stepper_motors_t *stepper_motor)
{
    gpio_set_output_toggle(stepper_motor->step_port, stepper_motor->step_pin);
}

/*
 * Convert a distance in millimeters to the number of edge transitions required to drive the stepper that far
 */
uint32_t stepper_distance_to_transitions(uint32_t distance)
{
    return 20*distance;
}

/*
 * Method to tell the stepper motors a relative distance to travel
 */
void stepper_go_to_position(uint32_t distance_x, uint32_t distance_y)
{
    // Disable the motors
    gpio_set_output_high(stepper_motor_x->enable_port, stepper_motor_x->enable_pin);
    stepper_motor_x->current_state = disabled;
    gpio_set_output_high(stepper_motor_y->enable_port, stepper_motor_x->enable_pin);
    stepper_motor_y->current_state = disabled;

    // Determine number of steps to reach the desired position
    stepper_motor_x->transitions_to_desired_x_position = stepper_distance_to_transitions(distance_x);
    stepper_motor_y->transitions_to_desired_y_position = stepper_distance_to_transitions(distance_y);

    // Enable the motors
    stepper_set_direction_counterclockwise(stepper_motor_x);
    stepper_motor_x->current_state = enabled;
    gpio_set_output_low(stepper_motor_x->enable_port, stepper_motor_x->enable_pin);
    stepper_set_direction_counterclockwise(stepper_motor_y);
    stepper_motor_y->current_state = enabled;
    gpio_set_output_low(stepper_motor_y->enable_port, stepper_motor_y->enable_pin);
}

/*
 * Interrupt handler to check if the desired number of steps are complete
 */
__interrupt void STEPPER_HANDLER(void)
{
    // Clear the interrupt flag
    TIMER0->ICR |= (TIMER_ICR_TATOCINT);

    // Move stepper 1 if it needs to
    if (stepper_motor_x->transitions_to_desired_x_position > 0) {
        stepper_edge_transition(stepper_motor_x);
        stepper_motor_x->transitions_to_desired_x_position -= 1;
    } else {
        stepper_motor_x->current_state = disabled;
    }

    // Move stepper 2 if it needs to
    if (stepper_motor_y->transitions_to_desired_y_position > 0) {
        stepper_edge_transition(stepper_motor_y);
        stepper_motor_y->transitions_to_desired_y_position -= 1;
    } else {
        stepper_motor_y->current_state = disabled;
    }
}

// End steppermotors.c
