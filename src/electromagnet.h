/**
 * @file electromagnet.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control an electromagnet driven by a BD62130AEFJ-E2
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef ELECTROMAGNET_H_
#define ELECTROMAGNET_H_

#include "msp.h"
#include "command_queue.h"
#include "pwm.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>

// Input mode table:
//  IN2 | IN1
//   0  |  0     <=> STOP
//   0  |  1     <=> FORWARD
//   1  |  0     <=> REVERSE
//   1  |  1     <=> BRAKE

// Electromagnet command struct
typedef struct electromagnet_command_t {
    command_t command;
    peripheral_state_t desired_state;   // Whether the magnet should be turned on or off
} electromagnet_command_t;

// Public functions
void electromagnet_init(void);

// Command Functions
electromagnet_command_t* electromagnet_build_command(peripheral_state_t desired_state);
void electromagnet_entry(command_t* command);
bool electromagnet_is_done(command_t* command);

#ifdef E_MAG_DEBUG
void electromagnet_attract(void);
void electromagnet_repel(void);
void electromagnet_disengage(void);
#endif

#endif /* ELECTROMAGNET_H_ */
