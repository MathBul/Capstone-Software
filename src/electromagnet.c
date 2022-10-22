/**
 * @file electromagnet.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control an electromagnet
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "electromagnet.h"

// Declare the electromagnet
electromagnet_t electromagnets[NUMBER_OF_ELECTROMAGNETS];
static electromagnet_t* electromagnet = &electromagnets[0];

/**
 * @brief Initialize the electromagnet
 */
void electromagnet_init()
{
    // Start with magnet off
    gpio_set_as_output(ELECTROMAGNET_INHIBIT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_high(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INHIBIT_PIN);

    // INPUT is used to send a signal to the magnet
    gpio_set_as_output(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
    gpio_set_output_low(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);

    // Configure the magnet struct
    electromagnet->inhibit_port = ELECTROMAGNET_INHIBIT_PORT;
    electromagnet->inhibit_pin  = ELECTROMAGNET_INHIBIT_PIN;
    electromagnet->input_port   = ELECTROMAGNET_INPUT_PORT;
    electromagnet->input_pin    = ELECTROMAGNET_INPUT_PIN;
}

/**
 * @brief Turn the electromagnet "on"
 */
void electromagnet_engage()
{
    gpio_set_output_high(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_high(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
}

/**
 * @brief Turn the electromagnet "off"
 */
void electromagnet_disengage()
{
    gpio_set_output_low(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_low(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
}

// Command Functions
void electromagnet_entry(command_t* command);
void electromagnet_action(command_t* command);
void electromagnet_exit(command_t* command);
bool electromagnet_is_done(command_t* command);


/* End electromagnet.c */
