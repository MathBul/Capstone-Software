#ifndef STEPPER_MOTORS_H_
#define STEPPER_MOTORS_H_

#include "msp.h"
#include "gpio.h"

// General stepper defines
#define NUMBER_OF_STEPPER_MOTORS            2
#define HALF_STEPS_TO_NEXT_ROW              500
#define STEPPER_HANDLER                     TIMER0A_IRQHandler

// Stepper 1 (x-axis)
#define STEPPER_1_ENABLE_PORT               (GPIOD)
#define STEPPER_1_ENABLE_PIN                (GPIO_PIN_2)
#define STEPPER_1_MS1_PORT                  (GPIOP)
#define STEPPER_1_MS1_PIN                   (GPIO_PIN_0)
#define STEPPER_1_MS2_PORT                  (GPIOP)
#define STEPPER_1_MS2_PIN                   (GPIO_PIN_1)
#define STEPPER_1_MS3_PORT                  (GPIOQ)
#define STEPPER_1_MS3_PIN                   (GPIO_PIN_0)
#define STEPPER_1_STEP_PORT                 (GPIOP)
#define STEPPER_1_STEP_PIN                  (GPIO_PIN_4)
#define STEPPER_1_DIR_PORT                  (GPION)
#define STEPPER_1_DIR_PIN                   (GPIO_PIN_5)
#define STEPPER_1_ID                        1

// Stepper 2 (y-axis)
#define STEPPER_2_ENABLE_PORT               (GPIOM)
#define STEPPER_2_ENABLE_PIN                (GPIO_PIN_0)
#define STEPPER_2_MS1_PORT                  (GPIOK)
#define STEPPER_2_MS1_PIN                   (GPIO_PIN_5)
#define STEPPER_2_MS2_PORT                  (GPIOK)
#define STEPPER_2_MS2_PIN                   (GPIO_PIN_4)
#define STEPPER_2_MS3_PORT                  (GPIOG)
#define STEPPER_2_MS3_PIN                   (GPIO_PIN_1)
#define STEPPER_2_STEP_PORT                 (GPIOM)
#define STEPPER_2_STEP_PIN                  (GPIO_PIN_2)
#define STEPPER_2_DIR_PORT                  (GPIOM)
#define STEPPER_2_DIR_PIN                   (GPIO_PIN_1)
#define STEPPER_2_ID                        2

// Information on whether the stepper motor is enabled or disabled
typedef enum {
    disabled, enabled
} stepper_motors_state_t;

// Information on whether the stepper motor is rotating clockise or counterclockwise
typedef enum {
    clockwise, counterclockwise
} stepper_motors_direction_t;

// Information on which column the arm is current above
typedef enum {
    col_a, col_b, col_c, col_d, col_e, col_f, col_g, col_h
} arm_x_position_t;

// Information on which row the arm is currently above
typedef enum {
    row_1, row_2, row_3, row_4, row_5, row_6, row_7, row_8
} arm_y_position_t;

// Stepper motor structure
typedef struct {
    uint8_t                 motor_id;                // ID for this stepper motor
    GPIO_Type*              dir_port;                // Port used to set the direction
    uint8_t                 dir_pin;                 // Pin used to set the direction
    GPIO_Type*              step_port;               // Port used to step the motor
    uint8_t                 step_pin;                // Pin used to step the motor
    GPIO_Type*              enable_port;             // Port used to enable/disable
    uint8_t                 enable_pin;              // Pin used to enable/disable the motor
    stepper_motors_state_t  current_state;           // Whether the motor is enabled/disabled
    arm_x_position_t        current_x_position;      // The current x-position of the motor in terms of board tiles
    arm_y_position_t        current_y_position;      // The current y-position of the motor in terms of board tiles
    arm_x_position_t        desired_x_position;      // The desired x-position of the motor in terms of board tiles
    arm_y_position_t        desired_y_position;      // The desired y-position of the motor in terms of board tiles
    uint32_t                transitions_to_desired_x_position; // 1/2 the number of periods to reach the desired x position
    uint32_t                transitions_to_desired_y_position; // 1/2 the number of periods to reach the desired y position
} stepper_motors_t;

// Stepper motor function declarations
void stepper_initialize_motors();
void stepper_toggle_direction(stepper_motors_t *stepper_motor);
void stepper_edge_transition(stepper_motors_t *stepper_motor);
void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor);
void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor);
void stepper_go_to_position(uint32_t distance_x, uint32_t distance_y);

#endif /* STEPPER_MOTORS_H_ */
