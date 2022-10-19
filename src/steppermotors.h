/**
 * @file steppermotors.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control stepper motors driven by DRV8824-Q1 chips
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef STEPPER_MOTORS_H_
#define STEPPER_MOTORS_H_

// Assumed board layout:
//  |-------|
//  | BOARD |
//  |-------X  <== Position (0,0), home
// ISR defined in gantry.c

#include "msp.h"
#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

// General stepper defines
#define NUMBER_OF_STEPPER_MOTORS            (2)
#define DISTANCE_BETWEEN_TILES              (47) //+0.625
#define STEPPER_TIMER                       (TIMER0)
#define STEPPER_HANDLER                     (TIMER0A_IRQHandler)

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
#define STEPPER_X_ID                        (0)

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
#define STEPPER_Y_ID                        (1)

// Stepper 3 (z-axis)
#define STEPPER_Z_ID                        (2)

// Tracks whether a stepper motor is enabled or disabled
typedef enum {
    disabled, enabled
} stepper_motors_state_t;

// Information on which column the arm is current above
typedef enum {
    pos_1, pos_2, pos_3, pos_4, pos_5, pos_6, pos_7, pos_8
} arm_position_t;

// Stepper motor structure
typedef struct {
    GPIO_Type*             dir_port;                   // Port used to set the direction
    uint8_t                dir_pin;                    // Pin used to set the direction
    GPIO_Type*             step_port;                  // Port used to step the motor
    uint8_t                step_pin;                   // Pin used to step the motor
    GPIO_Type*             enable_port;                // Port used to enable/disable
    uint8_t                enable_pin;                 // Pin used to enable/disable the motor
    stepper_motors_state_t current_state;              // Whether the motor is enabled/disabled
    int32_t                distance_to_home;           // Distance (in mm) to travel to home position
    arm_position_t         current_position;           // Current position of the motor in terms of board tiles
    uint32_t               transitions_to_desired_pos; // (2)*(#periods to desired position)
} stepper_motors_t;

// Public functions
void stepper_init_motors();
void stepper_disable_motor(uint8_t stepper_id);
void stepper_go_to_rel_position(int32_t distance_x, int32_t distance_y, int32_t distance_z);
void stepper_go_home();
bool stepper_arrived();
void stepper_take_action();
void stepper_stop_motors();
void stepper_resume_motors();
void stepper_reset();
void stepper_get_next_pos_commands();

// Private functions
static void stepper_toggle_direction(stepper_motors_t *stepper_motor);
static void stepper_edge_transition(stepper_motors_t *stepper_motor);
static void stepper_set_direction_clockwise(stepper_motors_t *stepper_motor);
static void stepper_set_direction_counterclockwise(stepper_motors_t *stepper_motor);
static void stepper_enable_motor(stepper_motors_t *stepper_motor);
static void stepper_wait();

#endif /* STEPPER_MOTORS_H_ */
