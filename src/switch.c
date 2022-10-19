/**
 * @file switch.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for all peripheral switches (limit, buttons, rocker, etc.)
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "switch.h"
#include "clock.h"

// Declare the switches
static switch_t switches;
static switch_t* p_switches = (&switches);

/**
 * @brief Initialize all buttons
 */
void buttons_init()
{
    // Configure GPIO for all switches
    gpio_set_as_input(SWITCH_PORT, BUTTON_PIN_MASK);
    gpio_set_as_input(SWITCH_PORT, LIMIT_PIN_MASK);

    // Initialize the button image
    utils_vport_init(switch_vport, SWITCH_PORT);
}

/**
 * @brief Shifts all switch-related bits from a vport bitfield to a local ordering
 * 
 * @param port_raw The raw value read from the switch image
 * @return uint8_t The reassigned value for the switch locally
 */
static uint8_t switch_shift_assign(uint8_t port_raw)
{
    uint8_t switch_reassigned = 0;
    switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_START_RESET_PIN, BUTTON_START_RESET);
    switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_COLOR_PIN, BUTTON_COLOR);
    switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_HOME_PIN, BUTTON_HOME);
    switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_X_PIN, LIMIT_X);
    switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_Y_PIN, LIMIT_Y);
    switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_Z_PIN, LIMIT_Z);
    return (switch_reassigned ^ SWITCH_INVERT_MASK);
}

uint8_t switch_read_all()
{
    // Read the switches and update the local struct
    uint8_t switches_raw        = utils_vport_read(switch_vport);
    p_switches->current_inputs  = switch_shift_assign(switches_raw);
    p_switches->edges           = (p_switches->current_inputs ^ p_switches->previous_inputs);
    p_switches->pos_transitions = (p_switches->current_inputs & p_switches->edges);
    p_switches->neg_transitions = ((~p_switches->current_inputs) & p_switches->edges);
    p_switches->previous_inputs = p_switches->current_inputs;

    // If the color button was pressed, flip the color latch. Default is white
    if ((p_switches->pos_transitions) & BUTTON_COLOR)
    {
        if (p_switches->color_latch == 'W')
        {
            p_switches->color_latch = 'B';
        }
        else
        {
            p_switches->color_latch = 'W';
        }
    }

    return p_switches->current_inputs;
}

/**
 * @brief Get the current color according to our switch data
 * 
 * @return char The color chosen, either 'W' or 'B'
 */
char switch_get_color()
{
    return p_switches->color_latch;
}

/* End buttons.c */
