/**
 * @file gantry.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Code to unite all other modules and provide simple interface for chess gameplay
 * @version 0.1
 * @date 2022-10-19
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef GANTRY_H_
#define GANTRY_H_

#include "clock.h"
#include "chessboard.h"
#include "command_queue.h"
#include "electromagnet.h"
#include "gpio.h"
#include "raspberrypi.h"
#include "sensornetwork.h"
#include "steppermotors.h"
#include "switch.h"
#include "uart.h"
#include "utils.h"

// Tracks the gantry position (can be used as column or row)
typedef enum {
    pos_1, pos_2, pos_3, pos_4, pos_5, pos_6, pos_7, pos_8
} gantry_position_t;

// Gantry command struct
typedef struct gantry_command_t {
    command_t command;
    char instruction[5];                // Full message from the Pi
    char instruction_type;              // Indicates: MOVE, CAPTURE, PROMOTION, ETC.
    gantry_position_t initial_pos[2];
    gantry_position_t final_pos[2];
} gantry_command_t;

// Command Functions
void gantry_entry(command_t* command);
void gantry_action(command_t* command);
void gantry_exit(command_t* command);
bool gantry_is_done(command_t* command);

#endif /* GANTRY_H_ */
