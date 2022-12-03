/**
 * @file switch.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for all peripheral switches (limits, buttons, toggles, etc.)
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

// Note on switches: 
//  - Assumes all switches are on the same physical port
//  - Creates a virtual port to access the physical port via imaging
//  - The SWITCH_HANDLER reads the virtual port and maps it to a local bitfield
//  - To move switches to different GPIO, change the GPIO macros below, no other changes required

#include "msp.h"
#include "gpio.h"
#include "utils.h"
#include "clock.h"
#include <stdint.h>

// General switch macros
#define SWITCH_TIMER                        (TIMER3)
#define SWITCH_HANDLER                      (TIMER3A_IRQHandler)
#define SWITCH_TEST_PORT                    (GPION)
#define SWITCH_TEST_PIN                     (GPIO_PIN_0)

// Button GPIO macros
#define BUTTON_START_PORT                   (GPIOF)
#define BUTTON_START_PIN                    (GPIO_PIN_2)
#define BUTTON_RESET_PORT                   (GPIOF)
#define BUTTON_RESET_PIN                    (GPIO_PIN_2)
#define BUTTON_HOME_PORT                    (GPIOM)
#define BUTTON_HOME_PIN                     (GPIO_PIN_3)
#define BUTTON_NEXT_TURN_PORT               (GPIOE)
#define BUTTON_NEXT_TURN_PIN                (GPIO_PIN_5)

// Toggle switch GPIO macros
#define COLOR_PORT                          (GPIOC)
#define COLOR_PIN                           (GPIO_PIN_6)

// Limit switch GPIO macros
#define LIMIT_PORT                          (GPIOK)
#define LIMIT_X_PIN                         (GPIO_PIN_2)
#define LIMIT_Y_PIN                         (GPIO_PIN_1)
#define LIMIT_Z_PIN                         (GPIO_PIN_0)

// Capture tile GPIO macros
#define CAPTURE_PORT                        (GPIOP)
#define CAPTURE_PIN                         (GPIO_PIN_1)

// Future-proofing switch GPIO macros
#define FUTURE_PROOF_PORT                   (GPIOM)
#define FUTURE_PROOF_1_PIN                  (GPIO_PIN_1)
#define FUTURE_PROOF_2_PIN                  (GPIO_PIN_0)
#define FUTURE_PROOF_3_PIN                  (GPIO_PIN_2)

// Shifts for where the bits will end up in the virtual port
#define BUTTON_START_SHIFT                  (0)
#define BUTTON_RESET_SHIFT                  (1)
#define BUTTON_HOME_SHIFT                   (2)
#define BUTTON_NEXT_TURN_SHIFT              (3)
#define TOGGLE_COLOR_SHIFT                  (4)
#define LIMIT_X_SHIFT                       (5)
#define LIMIT_Y_SHIFT                       (6)
#define LIMIT_Z_SHIFT                       (7)
#define SWITCH_CAPTURE_SHIFT                (8)
#define FUTURE_PROOF_1_SHIFT                (9)
#define FUTURE_PROOF_2_SHIFT                (10)
#define FUTURE_PROOF_3_SHIFT                (11)

// User-access macros
#define BUTTON_START_MASK                   (BITS16_MASK(BUTTON_START_SHIFT))
#define BUTTON_RESET_MASK                   (BITS16_MASK(BUTTON_RESET_SHIFT))
#define BUTTON_HOME_MASK                    (BITS16_MASK(BUTTON_HOME_SHIFT))
#define BUTTON_NEXT_TURN_MASK               (BITS16_MASK(BUTTON_NEXT_TURN_SHIFT))
#define TOGGLE_COLOR_MASK                   (BITS16_MASK(TOGGLE_COLOR_SHIFT))
#define LIMIT_X_MASK                        (BITS16_MASK(LIMIT_X_SHIFT))
#define LIMIT_Y_MASK                        (BITS16_MASK(LIMIT_Y_SHIFT))
#define LIMIT_Z_MASK                        (BITS16_MASK(LIMIT_Z_SHIFT))
#define SWITCH_CAPTURE_MASK                 (BITS16_MASK(SWITCH_CAPTURE_SHIFT))
#define FUTURE_PROOF_1_MASK                 (BITS16_MASK(FUTURE_PROOF_1_SHIFT))
#define FUTURE_PROOF_2_MASK                 (BITS16_MASK(FUTURE_PROOF_2_SHIFT))
#define FUTURE_PROOF_3_MASK                 (BITS16_MASK(FUTURE_PROOF_3_SHIFT))

// Inversion masks
#define BUTTON_MASK                         (BUTTON_START_MASK | BUTTON_RESET_MASK | BUTTON_HOME_MASK | BUTTON_NEXT_TURN_MASK)
#define LIMIT_MASK                          (LIMIT_X_MASK | LIMIT_Y_MASK | LIMIT_Z_MASK)
#define TOGGLE_MASK                         (TOGGLE_COLOR_MASK)
#define CAPTURE_MASK                        (SWITCH_CAPTURE_MASK)
#define FUTURE_PROOF_MASK                   (FUTURE_PROOF_1_MASK | FUTURE_PROOF_2_MASK | FUTURE_PROOF_3_MASK)
#define SWITCH_MASK                         (BUTTON_MASK | LIMIT_MASK | TOGGLE_MASK | CAPTURE_MASK | FUTURE_PROOF_MASK)

// Local representation of the switches
typedef struct {
    uint16_t current_inputs;
    uint16_t edges;
    uint16_t pos_transitions;
    uint16_t neg_transitions;
    uint16_t previous_inputs;
} switch_state_t;

// Virtual port for the switches
union utils_vport16_t switch_vport;

// Public functions
void switch_init();
uint16_t switch_get_reading();
void switch_test(uint16_t mask);

#endif /* SWITCHES_H_ */
