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
#include "delay.h"

// General gantry defines
#define GANTRY_TIMER                        (TIMER4)
#define GANTRY_HANDLER                      (TIMER4A_IRQHandler)



// Gantry command struct
typedef struct gantry_command_t {
    command_t command;
    chess_move_t move;              // All the info about a chess move
} gantry_command_t;

// Public functions
void gantry_init();

// Command Functions (reading user input)
void gantry_human_entry(command_t* command);
void gantry_human_action(command_t* command);
void gantry_human_exit(command_t* command);
bool gantry_human_is_done(command_t* command);

// Command Functions (preparing/performing moves)
void gantry_robot_entry(command_t* command);
void gantry_robot_action(command_t* command);
void gantry_robot_exit(command_t* command);
bool gantry_robot_is_done(command_t* command);

#endif /* GANTRY_H_ */
