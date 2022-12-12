/**
 * @file command_queue.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First-In, First-Out (FIFO) data structure for commands
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef COMMAND_QUEUE_H_
#define COMMAND_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Command queue defines
#define COMMAND_QUEUE_SIZE          (128) // Must be at least 1 and less than 65535

// Node type for the command queue
typedef struct command_t command_t;
struct command_t {
    void (*p_entry)(command_t* command);
    void (*p_action)(command_t* command);
    void (*p_exit)(command_t* command);
    bool (*p_is_done)(command_t* command);
};

// Function definitions
void command_queue_init(void);
bool command_queue_push(command_t* value);
bool command_queue_pop(command_t** p_value);
uint16_t command_queue_get_size(void);
bool command_queue_is_empty(void);
bool command_queue_clear(void);

#endif /* COMMAND_QUEUE_H_ */
