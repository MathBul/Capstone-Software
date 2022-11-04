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

// Math Behind Transitions:
//  Stepper performs 200 steps/revolution. We microstep to 1/2 steps, so 400 microsteps/revolution
//  Belt pitch is 2 mm, and rotor has 20 teeth, so 40mm/revolution
//  There are 2 transitions/microstep
//      ==> (2 transitions/microstep)*(400 microsteps/revolution)/(40mm/revolution) = 20 transitions/mm

#include "msp.h"
#include "gpio.h"
#include "clock.h"
#include "command_queue.h"
#include "switch.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// General stepper defines
#define NUMBER_OF_STEPPER_MOTORS            (3)
#define TRANSITIONS_PER_MM                  (20)
#define STEPPER_HOME_DISTANCE               (999)
#define STEPPER_HOME_VELOCITY               (1)

#define STEPPER_X_TIMER                     (TIMER0)
#define STEPPER_X_HANDLER                   (TIMER0A_IRQHandler)
#define STEPPER_X_INITIAL_PERIOD            (23999)

#define STEPPER_Y_TIMER                     (TIMER1)
#define STEPPER_Y_HANDLER                   (TIMER1A_IRQHandler)
#define STEPPER_Y_INITIAL_PERIOD            (23999)

#define STEPPER_Z_TIMER                     (TIMER2)
#define STEPPER_Z_HANDLER                   (TIMER2A_IRQHandler)
#define STEPPER_Z_INITIAL_PERIOD            (23999)


// Stepper X
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

// Stepper Y
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

// Stepper Z
#define STEPPER_Z_ENABLE_PORT               (GPIOF)
#define STEPPER_Z_ENABLE_PIN                (GPIO_PIN_1)
#define STEPPER_Z_MS1_PORT                  (GPIOF)
#define STEPPER_Z_MS1_PIN                   (GPIO_PIN_2)
#define STEPPER_Z_MS2_PORT                  (GPIOF)
#define STEPPER_Z_MS2_PIN                   (GPIO_PIN_3)
#define STEPPER_Z_MS3_PORT                  (GPIOG)
#define STEPPER_Z_MS3_PIN                   (GPIO_PIN_0)
#define STEPPER_Z_STEP_PORT                 (GPIOL)
#define STEPPER_Z_STEP_PIN                  (GPIO_PIN_4)
#define STEPPER_Z_DIR_PORT                  (GPIOL)
#define STEPPER_Z_DIR_PIN                   (GPIO_PIN_5)
#define STEPPER_Z_ID                        (2)

// Speeds
#define STEPPER_MAX_SPEED                   (250)      // mm/s
#define STEPPER_MIN_SPEED                   (100)      // mm/s

// Stepper motor structure
typedef struct {
    GPIO_Type*             dir_port;                   // Port used to set the direction
    uint8_t                dir_pin;                    // Pin used to set the direction
    GPIO_Type*             step_port;                  // Port used to step the motor
    uint8_t                step_pin;                   // Pin used to step the motor
    GPIO_Type*             enable_port;                // Port used to enable/disable
    uint8_t                enable_pin;                 // Pin used to enable/disable the motor
    peripheral_state_t     current_state;              // Whether the motor is enabled/disabled
    uint32_t               transitions_to_desired_pos; // (2)*(#periods to desired position)
    int8_t                 dir;                        // +/- 1 based on direction
    int32_t                current_pos;                // Distance (in transitions) along the axis. Measured from home position
    uint16_t               current_vel;                // Velocity (in register values) at the present moment
} stepper_motors_t;

// Stepper motor command structs
typedef struct stepper_rel_command_t {
    command_t command;
    int16_t rel_x;                                      // Distance to move in x (relative to current position) mm
    int16_t rel_y;                                      // Distance to move in y (relative to current position) mm
    int16_t rel_z;                                      // Distance to move in z (relative to current position) mm
    uint16_t v_x;                                       // Speed in x (direction determined by sign of the distance to move) mm/s
    uint16_t v_y;                                       // Speed in y (direction determined by sign of the distance to move) mm/s
    uint16_t v_z;                                       // Speed in z (direction determined by sign of the distance to move) mm/s
} stepper_rel_command_t;

typedef struct stepper_chess_command_t {
    command_t command;
    chess_file_t file;                                  // Distance to move in x (relative to current position) mm
    chess_rank_t rank;                                  // Distance to move in y (relative to current position) mm
    chess_piece_t piece;                                // Distance to move in z (relative to current position) mm
    uint16_t v_x;                                       // Speed in x (direction determined by sign of the distance to move) mm/s
    uint16_t v_y;                                       // Speed in y (direction determined by sign of the distance to move) mm/s
    uint16_t v_z;                                       // Speed in z (direction determined by sign of the distance to move) mm/s
} stepper_chess_command_t;

// Public functions
void stepper_init_motors();
void stepper_x_stop();
void stepper_y_stop();
void stepper_z_stop();

// Command Functions
stepper_rel_command_t* stepper_build_rel_command(int16_t rel_x, int16_t rel_y, int16_t rel_z, uint16_t v_x, uint16_t v_y, uint16_t v_z);
stepper_chess_command_t* stepper_build_chess_command(chess_file_t file, chess_rank_t rank, chess_piece_t piece, uint16_t v_x, uint16_t v_y, uint16_t v_z);
stepper_rel_command_t* stepper_build_home_xy_command();
stepper_rel_command_t* stepper_build_home_z_command();
void stepper_rel_entry(command_t* command);
void stepper_chess_entry(command_t* command);
void stepper_home_action(command_t* command);
void stepper_exit(command_t* command);
bool stepper_is_done(command_t* command);

#endif /* STEPPER_MOTORS_H_ */
