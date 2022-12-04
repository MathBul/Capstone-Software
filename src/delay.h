/**
 * @file delay.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a waiting command
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "msp.h"
#include "clock.h"
#include "command_queue.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// General delay macros
#define DELAY_TIMER                     (TIMER5)
#define DELAY_HANDLER                   (TIMER5A_IRQHandler)
#define DELAY_PERIOD                    (119999)

// Delay command struct
typedef struct delay_command_t {
    command_t command;
    uint32_t time_ms;
} delay_command_t;

// Command functions
delay_command_t* delay_build_command(uint16_t time_ms);
void delay_entry(command_t* command);
bool delay_is_done(command_t* command);

#endif /* DELAY_H_ */
