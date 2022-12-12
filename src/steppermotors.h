/**
 * @file steppermotors.h
 * @author Nick Cooney (npc4crc@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Provides functions to control stepper motors driven by DRV8824-Q1 chips
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef STEPPER_MOTORS_H_
#define STEPPER_MOTORS_H_

// Note on stepper motors:
//  - Motors are started/stopped by enable rather than sleep
//      - While sleep might save more power, it also requires delay before the first step after waking
//  - Assumes reset and sleep are connected to the same GPIO pin
//  - Rather than using the home output of the stepper, we drive until a limit switch is pressed, then backoff
//  - Assumed home position:
//             _
//             | ARM
//             |
//      |------X  <== Home, position (0,0)
//        BASE
//
// Transition math:
//  - Steppers {X,Y} perform 200 steps/revolution. We microstep to M steps, so 200*M microsteps/revolution
//  - Belt pitch is 2 mm, and rotor has 20 teeth, so 40mm/revolution
//  - There are 2 transitions/microstep
//      ==> (2 transitions/microstep)*(200*M microsteps/revolution)/(40mm/revolution) = 10*M transitions/mm
//
//  - Steppers {Z} perform 200 steps/revolution. We microstep to 1/8 steps, so 1600 microsteps/revolution
//  - Belt pitch is 2.5 mm, and rotor has 20 teeth, so 50mm/revolution
//  - There are 2 transitions/microstep
//      ==> (2 transitions/microstep)*(200*M microsteps/revolution)/(50mm/revolution) = 8*M transitions/mm
//
// Microstepping table:
//  MS2 | MS1 | MS0
//   0 |   0 |  0    <=> Full step
//   0 |   0 |  1    <=> 1/2 step
//   0 |   1 |  0    <=> 1/4 step
//   0 |   1 |  1    <=> 1/8 step
//   1 |   0 |  0    <=> 1/16 step
//   All Others      <=> 1/32 step

#include "msp.h"
#include "gpio.h"
#include "clock.h"
#include "command_queue.h"
#include "switch.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// Debugging motion profiling
#ifdef STEPPER_DEBUG
#include <stdio.h>

#define PROFILING_CHANNEL                   (UART_CHANNEL_0)
#endif

// General stepper defines
#define NUMBER_OF_STEPPER_MOTORS            (3)
#define MICROSTEP_LEVEL                     (8)
#define TRANSITIONS_PER_MM                  (10 * MICROSTEP_LEVEL)
#define TRANSITIONS_PER_MM_Z                (8 * MICROSTEP_LEVEL)
#define STEPPER_HOME_DISTANCE               (999)       // mm (arbitrary large value)
#define STEPPER_HOME_VELOCITY               (1)         // mm/s
#define STEPPER_MIN_SPEED                   (135)       // mm/s
#define STEPPER_MAX_SPEED                   (250)       // mm/s

// Common and microstepping GPIO
#define STEPPER_XYZ_NRESET_PORT             (GPIOE)
#define STEPPER_XYZ_NRESET_PIN              (GPIO_PIN_0)
#define STEPPER_XYZ_DECAY_PORT              (GPIOE)
#define STEPPER_XYZ_DECAY_PIN               (GPIO_PIN_1)
#define STEPPER_XY_MS0_PORT                 (GPIOD)
#define STEPPER_XY_MS0_PIN                  (GPIO_PIN_6)
#define STEPPER_XY_MS1_PORT                 (GPIOM)
#define STEPPER_XY_MS1_PIN                  (GPIO_PIN_4)
#define STEPPER_XY_MS2_PORT                 (GPIOM)
#define STEPPER_XY_MS2_PIN                  (GPIO_PIN_5)
#define STEPPER_Z_MS0_PORT                  (GPIOK)
#define STEPPER_Z_MS0_PIN                   (GPIO_PIN_6)
#define STEPPER_Z_MS1_PORT                  (GPIOH)
#define STEPPER_Z_MS1_PIN                   (GPIO_PIN_1)
#define STEPPER_Z_MS2_PORT                  (GPIOH)
#define STEPPER_Z_MS2_PIN                   (GPIO_PIN_0)

// Stepper X
#define STEPPER_X_NENABLE_PORT              (GPIOB)
#define STEPPER_X_NENABLE_PIN               (GPIO_PIN_2)
#define STEPPER_X_STEP_PORT                 (GPIOC)
#define STEPPER_X_STEP_PIN                  (GPIO_PIN_7)
#define STEPPER_X_DIR_PORT                  (GPIOB)
#define STEPPER_X_DIR_PIN                   (GPIO_PIN_3)
#define STEPPER_X_NFAULT_PORT               (GPIOD)
#define STEPPER_X_NFAULT_PIN                (GPIO_PIN_3)
#define STEPPER_X_NHOME_PORT                (GPIOD)
#define STEPPER_X_NHOME_PIN                 (GPIO_PIN_2)
#define STEPPER_X_ID                        (0)
#define STEPPER_X_MAX_V                     (3000 * MICROSTEP_LEVEL)    // transitions/s
#define STEPPER_X_MAX_A                     (9500 * MICROSTEP_LEVEL)    // transitions/s/s
#define STEPPER_X_TIMER                     (TIMER0)
#define STEPPER_X_HANDLER                   (TIMER0A_IRQHandler)
#define STEPPER_X_INITIAL_PERIOD            ((48000 / MICROSTEP_LEVEL) - 1)

// Stepper Y
#define STEPPER_Y_NENABLE_PORT              (GPIOE)
#define STEPPER_Y_NENABLE_PIN               (GPIO_PIN_3)
#define STEPPER_Y_STEP_PORT                 (GPIOD)
#define STEPPER_Y_STEP_PIN                  (GPIO_PIN_7)
#define STEPPER_Y_DIR_PORT                  (GPIOE)
#define STEPPER_Y_DIR_PIN                   (GPIO_PIN_2)
#define STEPPER_Y_NFAULT_PORT               (GPIOG)
#define STEPPER_Y_NFAULT_PIN                (GPIO_PIN_0)
#define STEPPER_Y_NHOME_PORT                (GPIOF)
#define STEPPER_Y_NHOME_PIN                 (GPIO_PIN_3)
#define STEPPER_Y_ID                        (1)
#define STEPPER_Y_MAX_V                     (3000 * MICROSTEP_LEVEL)    // transitions/s
#define STEPPER_Y_MAX_A                     (9500 * MICROSTEP_LEVEL)    // transitions/s/s
#define STEPPER_Y_TIMER                     (TIMER1)
#define STEPPER_Y_HANDLER                   (TIMER1A_IRQHandler)
#define STEPPER_Y_INITIAL_PERIOD            ((48000 / MICROSTEP_LEVEL) - 1)

// Stepper Z
#define STEPPER_Z_NENABLE_PORT              (GPION)
#define STEPPER_Z_NENABLE_PIN               (GPIO_PIN_5)
#define STEPPER_Z_STEP_PORT                 (GPION)
#define STEPPER_Z_STEP_PIN                  (GPIO_PIN_4)
#define STEPPER_Z_DIR_PORT                  (GPIOP)
#define STEPPER_Z_DIR_PIN                   (GPIO_PIN_4)
#define STEPPER_Z_NFAULT_PORT               (GPIOB)
#define STEPPER_Z_NFAULT_PIN                (GPIO_PIN_4)
#define STEPPER_Z_NHOME_PORT                (GPIOB)
#define STEPPER_Z_NHOME_PIN                 (GPIO_PIN_5)
#define STEPPER_Z_ID                        (2)
#define STEPPER_Z_MAX_V                     (500 * MICROSTEP_LEVEL)     // transitions/s
#define STEPPER_Z_MAX_A                     (2000 * MICROSTEP_LEVEL)    // transitions/s/s
#define STEPPER_Z_TIMER                     (TIMER2)
#define STEPPER_Z_HANDLER                   (TIMER2A_IRQHandler)
#define STEPPER_Z_INITIAL_PERIOD            ((48000 / MICROSTEP_LEVEL) - 1)

// Stepper motor struct
typedef struct {
    TIMER0_Type*           timer;                      // Timer used for motion profiling
    GPIO_Type*             dir_port;                   // Port used to set the direction
    uint8_t                dir_pin;                    // Pin used to set the direction
    GPIO_Type*             step_port;                  // Port used to step the motor
    uint8_t                step_pin;                   // Pin used to step the motor
    GPIO_Type*             nenable_port;               // Port used to enable/disable
    uint8_t                nenable_pin;                // Pin used to enable/disable the motor
    GPIO_Type*             nfault_port;                // Port used by the stepper to indicate a fault
    uint8_t                nfault_pin;                 // Pin used by the stepper to indicate a fault
    GPIO_Type*             nhome_port;                 // Port used by the stepper when home
    uint8_t                nhome_pin;                  // Pin used by the stepper when home
    peripheral_state_t     current_state;              // Whether the motor is enabled/disabled
    uint32_t               transitions_to_desired_pos; // (2)*(#periods to desired position)
    int8_t                 dir;                        // +/- 1 to indicate direction
    int32_t                current_pos;                // Distance (in transitions) along the axis, from home position
    uint16_t               current_vel;                // Velocity (in CCR values) at the present moment
    int32_t                x_1;                        // Point where the speed plateaus (in transitions)
    int32_t                x_2;                        // Point where the speed starts decreasing (in transitions)
    uint16_t               max_accel;                  // Max value to adjust the clock period to accel/deccel
    uint8_t                motor_id;                   // Unique identifier for each motor
#ifdef STEPPER_DEBUG
    uint32_t               time_elapsed;
#endif
} stepper_motors_t;

// Stepper motor command structs
typedef struct stepper_rel_command_t {
    command_t command;
    int16_t rel_x;                                      // Distance to move in X (relative to current position) mm
    int16_t rel_y;                                      // Distance to move in Y (relative to current position) mm
    int16_t rel_z;                                      // Distance to move in Z (relative to current position) mm
    uint16_t v_x;                                       // Speed in X (direction determined by sign of the distance to move) mm/s
    uint16_t v_y;                                       // Speed in Y (direction determined by sign of the distance to move) mm/s
    uint16_t v_z;                                       // Speed in Z (direction determined by sign of the distance to move) mm/s
} stepper_rel_command_t;

typedef struct stepper_chess_command_t {
    command_t command;
    chess_file_t file;                                  // Location to move to in X mm
    chess_rank_t rank;                                  // Location to move to in Y mm
    chess_piece_t piece;                                // Location to move to in Z mm
    uint16_t v_x;                                       // Speed in X (direction determined by sign of the distance to move) mm/s
    uint16_t v_y;                                       // Speed in Y (direction determined by sign of the distance to move) mm/s
    uint16_t v_z;                                       // Speed in Z (direction determined by sign of the distance to move) mm/s
} stepper_chess_command_t;

// Public functions
void stepper_init_motors(void);
void stepper_x_stop(void);
void stepper_y_stop(void);
void stepper_z_stop(void);
bool stepper_x_has_fault(void);
bool stepper_y_has_fault(void);
bool stepper_z_has_fault(void);

// Command Functions
stepper_rel_command_t* stepper_build_rel_command(int16_t rel_x, int16_t rel_y, int16_t rel_z, uint16_t v_x, uint16_t v_y, uint16_t v_z);
stepper_chess_command_t* stepper_build_chess_xy_command(chess_file_t file, chess_rank_t rank, uint16_t v_x, uint16_t v_y);
stepper_chess_command_t* stepper_build_chess_z_command(chess_piece_t piece, uint16_t v_z);
stepper_rel_command_t* stepper_build_home_xy_command(void);
stepper_rel_command_t* stepper_build_home_z_command(void);
void stepper_rel_entry(command_t* command);
void stepper_chess_entry(command_t* command);
void stepper_home_entry(command_t* command);
void stepper_exit(command_t* command);
bool stepper_is_done(command_t* command);

#endif /* STEPPER_MOTORS_H_ */
