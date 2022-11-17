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

// Private functions
void electromagnet_engage();
void electromagnet_disengage();

// Declare the electromagnet
electromagnet_t electromagnet;
static electromagnet_t* p_electromagnet = &electromagnet;

/**
 * @brief Initialize the electromagnet
 */
void electromagnet_init()
{
    // Start with magnet off
    gpio_set_as_output(ELECTROMAGNET_INHIBIT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_high(ELECTROMAGNET_INHIBIT_PORT, ELECTROMAGNET_INHIBIT_PIN);

    // INPUT is used to send a signal to the magnet
    gpio_set_as_output(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
    gpio_set_output_low(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);

    // Configure the magnet struct
    p_electromagnet->inhibit_port = ELECTROMAGNET_INHIBIT_PORT;
    p_electromagnet->inhibit_pin  = ELECTROMAGNET_INHIBIT_PIN;
    p_electromagnet->input_port   = ELECTROMAGNET_INPUT_PORT;
    p_electromagnet->input_pin    = ELECTROMAGNET_INPUT_PIN;
}

/**
 * @brief Turn the electromagnet "on"
 */
void electromagnet_engage()
{
    gpio_set_output_low(ELECTROMAGNET_INHIBIT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_high(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
}

/**
 * @brief Turn the electromagnet "off"
 */
void electromagnet_disengage()
{
    gpio_set_output_high(ELECTROMAGNET_INHIBIT_PORT, ELECTROMAGNET_INHIBIT_PIN);
    gpio_set_output_low(ELECTROMAGNET_INPUT_PORT, ELECTROMAGNET_INPUT_PIN);
}

/* Command Functions */

/**
 * @brief Builds the electromagnet command
 *
 * @param desired_state One of {enabled, disabled}
 * @return Pointer to the command object
 */
electromagnet_command_t* electromagnet_build_command(peripheral_state_t desired_state)
{
    // The thing to return
    electromagnet_command_t* p_command = (electromagnet_command_t*) malloc(sizeof(electromagnet_command_t));

    // Functions
    p_command->command.p_entry   = &electromagnet_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &electromagnet_is_done;

    // Data
    p_command->desired_state = desired_state;

    return p_command;
}

/**
 * @brief Enables or disables the electromagnet
 *
 * @param command Pointer to the electromagnet command
 */
void electromagnet_entry(command_t* command)
{
    electromagnet_command_t* p_command = (electromagnet_command_t*) command;

    // Get the desired settings
    peripheral_state_t desired_state = p_command->desired_state;

    // Enable or disable the electromagnet as desired
    if (desired_state == enabled)
    {
        electromagnet_engage();
    }
    else
    {
        electromagnet_disengage();
    }
}

/**
 * @brief Always returns true since no action is done
 *
 * @param command Pointer to the electromagnet command
 * @return true Always
 */
bool electromagnet_is_done(command_t* command)
{
    return true;
}

/* End electromagnet.c */
