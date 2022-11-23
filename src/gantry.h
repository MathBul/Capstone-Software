/**
 * @file gantry.h
 * @author Nick Cooney (npc4crc@virginia.edu), Eli Jelesko (ebj5hec@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
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
#include "delay.h"
#include "electromagnet.h"
#include "gpio.h"
#include "led.h"
#include "raspberrypi.h"
#include "sensornetwork.h"
#include "steppermotors.h"
#include "switch.h"
#include "uart.h"
#include "utils.h"

// Flags for main
extern bool sys_fault;

// General gantry defines
#define GANTRY_TIMER                        (TIMER4)
#define GANTRY_HANDLER                      (TIMER4A_IRQHandler)

// Gantry command struct
typedef struct gantry_command_t {
    command_t command;
    chess_move_t move;              // All the info about the move the robot will make
    char robot_move_uci[5];         // The move the robot will make in UCI notation
    game_status_t game_status;      // The current state of the game, as reported by the Pi (ongoing, ended)
    char move_to_send[5];           // The human's move in UCI notation
} gantry_command_t;

// Public functions
void gantry_init(void);
void gantry_reset(void);

// Command Functions (reading user input)
gantry_command_t* gantry_human_build_command(void);
void gantry_human_entry(command_t* command);
void gantry_human_action(command_t* command);
void gantry_human_exit(command_t* command);
bool gantry_human_is_done(command_t* command);

// Command Functions (sending the user's move, verifying transmission)
gantry_command_t* gantry_comm_build_command(char move[5]);
void gantry_comm_entry(command_t* command);
void gantry_comm_action(command_t* command);
void gantry_comm_exit(command_t* command);
bool gantry_comm_is_done(command_t* command);

// Command Functions (preparing/performing moves)
gantry_command_t* gantry_robot_build_command(void);
void gantry_robot_entry(command_t* command);
void gantry_robot_action(command_t* command);
void gantry_robot_exit(command_t* command);
bool gantry_robot_is_done(command_t* command);

// Command Functions (homing the system)
gantry_command_t* gantry_home_build_command(void);
void gantry_home_entry(command_t* command);
void gantry_home_action(command_t* command);
void gantry_home_exit(command_t* command);
bool gantry_home_is_done(command_t* command);

#endif /* GANTRY_H_ */
