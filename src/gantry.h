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

// Note on system flow:
//  - gantry_human_command:
//      - Read board
//      - If capture tile pressed, save snapshot
//      - When "end turn" pressed, update the board state
//      - If the move was legal, load a gantry_comm_command
//      - Else, turn on error LED and load a gantry_human_command
//  - gantry_comm_command:
//      - Transmit the move
//      - If ACK received, load a gantry_robot_command
//      - Else, wait 5 seconds and retransmit
//  - gantry_robot_command:
//      - Turn on the robot moving LED
//      - Make the move specified
//      - Turn off the robot moving LED
//      - If the game is ONGOING, turn on human moving LED and load a gantry_human_command
//      - Else, turn on a white LED and load no further commands (wait for reset)
// - TODO: Handle case of RPi does not receive ACK?

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

// General gantry defines
#define GANTRY_TIMER                        (TIMER4)
#define GANTRY_HANDLER                      (TIMER4A_IRQHandler)

// Communication defines
#define COMM_TIMER                          (TIMER7)
#define COMM_HANDLER                        (TIMER7A_IRQHandler)
#define COMM_TIMEOUT                        (600000000)

// Motor speed defines
#define MOTORS_MOVE_V_X                     (1)
#define MOTORS_MOVE_V_Y                     (1)
#define MOTORS_MOVE_V_Z                     (1)

// Shared flags
extern bool sys_fault;

// Gantry command structs
typedef struct gantry_command_t {
    command_t command;
} gantry_command_t;

typedef struct gantry_robot_command_t {
    command_t command;
    char move_uci[5];           // The move the RPi sent
    chess_move_t move;          // How to move the motors
    game_status_t game_status;  // The current state of the game, as reported by the Pi (ongoing, ended)
} gantry_robot_command_t;

typedef struct gantry_comm_command_t {
    command_t command;
    char move_to_send[5];           // The human's move in UCI notation
} gantry_comm_command_t;

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
gantry_comm_command_t* gantry_comm_build_command(char move[5]);
void gantry_comm_entry(command_t* command);
void gantry_comm_action(command_t* command);
void gantry_comm_exit(command_t* command);
bool gantry_comm_is_done(command_t* command);

// Command Functions (preparing/performing moves)
gantry_robot_command_t* gantry_robot_build_command(void);
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
