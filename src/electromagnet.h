/**
 * @file electromagnet.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control an electromagnet driven by a BTN8962TA chip
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef ELECTROMAGNET_H_
#define ELECTROMAGNET_H_

#include "msp.h"
#include "gpio.h"
#include "command_queue.h"
#include <stdint.h>
#include <stdlib.h>

// General electromagnet defines
#define ELECTROMAGNET_INHIBIT_PORT          (GPIOQ)
#define ELECTROMAGNET_INHIBIT_PIN           (GPIO_PIN_2)
#define ELECTROMAGNET_INPUT_PORT            (GPIOQ)
#define ELECTROMAGNET_INPUT_PIN             (GPIO_PIN_3)

// Electromagnet struct
typedef struct {
    GPIO_Type* inhibit_port;            // Port used to enable/disable the EM
    uint8_t    inhibit_pin;             // Pin used to enable/disable the EM
    GPIO_Type* input_port;              // Port used to send high/low signals to the EM
    uint8_t    input_pin;               // Pin used to send high/low signals to the EM
} electromagnet_t;

// Electromagnet command struct
typedef struct electromagnet_command_t {
    command_t command;
    peripheral_state_t desired_state;   // Whether the magnet should be turned on or off
} electromagnet_command_t;

// Public functions
void electromagnet_init();

// Command Functions
electromagnet_command_t* electromagnet_build_command(peripheral_state_t desired_state);
void electromagnet_entry(command_t* command);
bool electromagnet_is_done(command_t* command);

#endif /* ELECTROMAGNET_H_ */
