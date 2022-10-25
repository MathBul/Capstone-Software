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

#include "msp.h"
#include "gpio.h"
#include "command_queue.h"
#include <stdint.h>
#include <stdbool.h>

// General stepper defines
#define NUMBER_OF_STEPPER_MOTORS            (3)
#define STEPPER_X_TIMER                     (TIMER0)
#define STEPPER_X_HANDLER                   (TIMER0A_IRQHandler)

// Stepper 1 (x-axis)
#define STEPPER_X_ENABLE_PORT               (GPIOD)
#define STEPPER_X_ENABLE_PIN                (GPIO_PIN_2)
#define STEPPER_X_MS1_PORT                  (GPIOP)
#define STEPPER_X_MS1_PIN                   (GPIO_PIN_0)
#define STEPPER_X_MS2_PORT                  (GPIOP)
#define STEPPER_X_MS2_PIN                   (GPIO_PIN_1)
#define STEPPER_X_MS3_PORT                  (GPIOQ)
#define STEPPER_X_MS3_PIN                   (GPIO_PIN_0)
#define STEPPER_X_STEP_PORT                 (GPIOP)
#define STEPPER_X_STEP_PIN                  (GPIO_PIN_4)
#define STEPPER_X_DIR_PORT                  (GPION)
#define STEPPER_X_DIR_PIN                   (GPIO_PIN_5)
#define STEPPER_X_ID                        (0)

// Stepper 2 (y-axis)
#define STEPPER_Y_ENABLE_PORT               (GPIOM)
#define STEPPER_Y_ENABLE_PIN                (GPIO_PIN_0)
#define STEPPER_Y_MS1_PORT                  (GPIOK)
#define STEPPER_Y_MS1_PIN                   (GPIO_PIN_5)
#define STEPPER_Y_MS2_PORT                  (GPIOK)
#define STEPPER_Y_MS2_PIN                   (GPIO_PIN_4)
#define STEPPER_Y_MS3_PORT                  (GPIOG)
#define STEPPER_Y_MS3_PIN                   (GPIO_PIN_1)
#define STEPPER_Y_STEP_PORT                 (GPIOM)
#define STEPPER_Y_STEP_PIN                  (GPIO_PIN_2)
#define STEPPER_Y_DIR_PORT                  (GPIOM)
#define STEPPER_Y_DIR_PIN                   (GPIO_PIN_1)
#define STEPPER_Y_ID                        (1)

// Stepper 3 (z-axis)
#define STEPPER_Z_ENABLE_PORT               (GPIOM)
#define STEPPER_Z_ENABLE_PIN                (GPIO_PIN_0)
#define STEPPER_Z_MS1_PORT                  (GPIOK)
#define STEPPER_Z_MS1_PIN                   (GPIO_PIN_5)
#define STEPPER_Z_MS2_PORT                  (GPIOK)
#define STEPPER_Z_MS2_PIN                   (GPIO_PIN_4)
#define STEPPER_Z_MS3_PORT                  (GPIOG)
#define STEPPER_Z_MS3_PIN                   (GPIO_PIN_1)
#define STEPPER_Z_STEP_PORT                 (GPIOM)
#define STEPPER_Z_STEP_PIN                  (GPIO_PIN_2)
#define STEPPER_Z_DIR_PORT                  (GPIOM)
#define STEPPER_Z_DIR_PIN                   (GPIO_PIN_1)
#define STEPPER_Z_ID                        (2)

// Tracks whether a stepper motor is enabled or disabled
typedef enum {
    disabled, enabled
} stepper_motors_state_t;

// Stepper motor structure
typedef struct {
    GPIO_Type*             dir_port;                   // Port used to set the direction
    uint8_t                dir_pin;                    // Pin used to set the direction
    GPIO_Type*             step_port;                  // Port used to step the motor
    uint8_t                step_pin;                   // Pin used to step the motor
    GPIO_Type*             enable_port;                // Port used to enable/disable
    uint8_t                enable_pin;                 // Pin used to enable/disable the motor
    stepper_motors_state_t current_state;              // Whether the motor is enabled/disabled
    uint32_t               transitions_to_desired_pos; // (2)*(#periods to desired position)
    int32_t                distance_to_home;           // Distance (in mm) to travel to home position
} stepper_motors_t;

// Stepper motor command struct
typedef struct stepper_command_t {
    command_t command;
    int16_t rel_x; // The distance to move in x (relative to current position)
    int16_t rel_y; // The distance to move in y (relative to current position)
    int16_t rel_z; // The distance to move in z (relative to current position)
    uint16_t v_x;  // The speed in x (direction is determined by the sign of the distance to move)
    uint16_t v_y;  // The speed in y (direction is determined by the sign of the distance to move)
    uint16_t v_z;  // The speed in z (direction is determined by the sign of the distance to move)
} stepper_command_t;

// Public functions
void stepper_init_motors();
// void stepper_stop_motors();      TODO
// void stepper_resume_motors();    TODO

// Command Functions
void stepper_entry(command_t* command);
void stepper_action(command_t* command);
void stepper_exit(command_t* command);
bool stepper_is_done(command_t* command);

#endif /* STEPPER_MOTORS_H_ */
