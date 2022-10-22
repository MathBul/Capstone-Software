/**
 * @file stepper_move_command.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Executes a stepper move
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef STEPPER_MOVE_COMMAND_H_
#define STEPPER_MOVE_COMMAND_H_

#include "command_queue.h"

typedef struct stepper_move_command_t {
    command_t command;
    uint32_t num_transitions;

}stepper_move_command_t;



#endif /* SRC_STEPPER_MOVE_COMMAND_H_ */
