/*
 * delay.h
 *
 *  Created on: Oct 30, 2022
 *      Author: EJ
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

#include "msp.h"
#include "clock.h"
#include "command_queue.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#define DELAY_TIMER                     (TIMER5)
#define DELAY_HANDLER                   (TIMER5A_IRQHandler)
#define DELAY_PERIOD                    (23999)

typedef struct delay_command_t {
    command_t command;
    uint32_t time_ms;
} delay_command_t;

delay_command_t* delay_build_command(uint16_t time_ms);
void delay_entry(command_t* command);
void delay_action(command_t* command);
void delay_exit(command_t* command);
bool delay_is_done(command_t* command);


#endif /* SRC_DELAY_H_ */
