/**
 * @file switch.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for all peripheral switches (limit, buttons, rocker, etc.)
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

// Note on use: 
//  - Assumes all switches are on the same physical port
//  - Creates a virtual port to access the physical port via imaging
//  - The SWITCH_HANDLER reads the virtual port and maps it to a local bitfield
//  - To move switches to different GPIO, change the GPIO macros below, no other changes required
// ISR defined in gantry.c

#include "msp.h"
#include "gpio.h"
#include "utils.h"
#include "clock.h"
#include <stdint.h>

// Switch GPIO macros
#define SWITCH_PORT                         (GPIOD)         // TODO: Update when known
#define BUTTON_RESET_PIN                    (GPIO_PIN_0)    // TODO: Update when known
#define BUTTON_END_TURN_PIN                 (GPIO_PIN_1)    // TODO: Update when known
#define BUTTON_ESTOP_PIN                    (GPIO_PIN_2)    // TODO: Update when known
#define BUTTON_HOME_PIN                     (GPIO_PIN_3)    // TODO: Update when known
#define LIMIT_X_PIN                         (GPIO_PIN_4)    // TODO: Update when known
#define LIMIT_Y_PIN                         (GPIO_PIN_5)    // TODO: Update when known
#define LIMIT_Z_PIN                         (GPIO_PIN_6)    // TODO: Update when known
#define ROCKER_COLOR_PIN                    (GPIO_PIN_7)    // TODO: Update when known
#define BUTTON_PIN_MASK                     (BUTTON_RESET_PIN | BUTTON_END_TURN_PIN | BUTTON_ESTOP_PIN | BUTTON_HOME_PIN)
#define LIMIT_PIN_MASK                      (LIMIT_X_PIN | LIMIT_Y_PIN | LIMIT_Z_PIN)
#define ROCKER_PIN_MASK                     (ROCKER_COLOR_PIN)

// General switch macros
#define SWITCH_TIMER                        (TIMER3)
#define SWITCH_HANDLER                      (TIMER3A_IRQHandler)
#define SWITCH_MASK                         (BUTTON_PIN_MASK | LIMIT_PIN_MASK | ROCKER_PIN_MASK)

// Button macros
#define BUTTON_START_RESET                  (BITS8_MASK(0))
#define BUTTON_ESTOP                        (BITS8_MASK(1))
#define BUTTON_HOME                         (BITS8_MASK(2))
#define BUTTON_END_TURN                     (BITS8_MASK(3))

// Limit switch macros
#define LIMIT_X                             (BITS8_MASK(4))
#define LIMIT_Y                             (BITS8_MASK(5))
#define LIMIT_Z                             (BITS8_MASK(6))

// Rocker macros
#define ROCKER_COLOR                        (BITS8_MASK(7))

// Storage for the switches
typedef struct switch_name_t {
    uint8_t limit_x : 1;
    uint8_t limit_y : 1;
    uint8_t limit_z : 1;
    uint8_t reserved_1 : 1;
    uint8_t reserved_2 : 1;
    uint8_t reserved_3 : 1;
    uint8_t reserved_4 : 1;
    uint8_t reserved_5 : 1;
} switch_name_t;

typedef union switches_t {
    switch_name named;
    uint8_t raw;
}switches_t;

// Local representation of the switches
typedef struct {
    uint8_t current_inputs;
    uint8_t edges;
    uint8_t pos_transitions;
    uint8_t neg_transitions;
    uint8_t previous_inputs;
    char    color_latch;
} switch_state_t;

// Virtual port for the switches
union utils_vport_t switch_vport;

// Public functions
void switch_init();

#endif /* SWITCHES_H_ */
