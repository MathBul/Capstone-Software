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
#include "electromagnet.h"
#include "raspberrypi.h"
#include "sensornetwork.h"
#include "steppermotors.h"
#include "switch.h"
#include "command_queue.h"


// Command Functions
void gantry_entry(command_t* command);
void gantry_action(command_t* command);
void gantry_exit(command_t* command);
bool gantry_is_done(command_t* command);

#endif /* GANTRY_H_ */
