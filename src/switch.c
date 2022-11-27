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

// Private functions
static uint16_t switch_shift_assign();

// Declare the switches
static switch_state_t switches;
static switch_state_t* p_switches = (&switches);

/**
 * @brief Initialize all buttons
 */
void switch_init()
{
    // Configure GPIO for all buttons
    gpio_set_as_input(BUTTON_START_PORT, BUTTON_START_PIN);
    gpio_set_as_input(BUTTON_RESET_PORT, BUTTON_RESET_PIN);
    gpio_set_as_input(BUTTON_HOME_PORT, BUTTON_HOME_PIN);
    gpio_set_as_input(BUTTON_NEXT_TURN_PORT, BUTTON_NEXT_TURN_PIN);

    // Configure GPIO for all toggle switches
    gpio_set_as_input(COLOR_PORT, COLOR_PIN);

    // Configure GPIO for all limit switches
    gpio_set_as_input(LIMIT_PORT, (LIMIT_X_PIN | LIMIT_Y_PIN | LIMIT_Z_PIN));

    // Configure GPIO for the capture tile
    gpio_set_as_input(CAPTURE_PORT, CAPTURE_PIN);
    
    // Configure GPIO for all future-proofing switches
    gpio_set_as_input(FUTURE_PROOF_PORT, (FUTURE_PROOF_1_PIN | FUTURE_PROOF_2_PIN | FUTURE_PROOF_3_PIN));

    // Start the ISR timer
    clock_start_timer(SWITCH_TIMER);
}

/**
 * @brief Gets the current switch readings
 *
 * @return The switch readings
 */
uint16_t switch_get_reading()
{
    return p_switches->current_inputs;
}

/* Interrupts */

/**
 * @brief Shifts all switch-related bits to a local ordering
 * 
 * @return The reassigned value for the switch locally
 */
static uint16_t switch_shift_assign()
{
    uint16_t switch_reassigned = 0;
    switch_reassigned |= (gpio_read_input(BUTTON_START_PORT, BUTTON_START_PIN)          << BUTTON_START_SHIFT);
    switch_reassigned |= (gpio_read_input(BUTTON_RESET_PORT, BUTTON_RESET_PIN)          << BUTTON_RESET_SHIFT);
    switch_reassigned |= (gpio_read_input(BUTTON_HOME_PORT, BUTTON_HOME_PIN)            << BUTTON_HOME_SHIFT);
    switch_reassigned |= (gpio_read_input(BUTTON_NEXT_TURN_PORT, BUTTON_NEXT_TURN_PIN)  << BUTTON_NEXT_TURN_SHIFT);
    switch_reassigned |= (gpio_read_input(COLOR_PORT, COLOR_PIN)                        << TOGGLE_COLOR_SHIFT);
    switch_reassigned |= (gpio_read_input(LIMIT_PORT, LIMIT_X_PIN)                      << LIMIT_X_SHIFT);
    switch_reassigned |= (gpio_read_input(LIMIT_PORT, LIMIT_Y_PIN)                      << LIMIT_Y_SHIFT);
    switch_reassigned |= (gpio_read_input(LIMIT_PORT, LIMIT_Z_PIN)                      << LIMIT_Z_SHIFT);
    uint8_t temp1 = gpio_read_input(CAPTURE_PORT, CAPTURE_PIN);
    uint8_t temp2 = (temp1 << SWITCH_CAPTURE_SHIFT);
    switch_reassigned |= (gpio_read_input(CAPTURE_PORT, CAPTURE_PIN)                    << SWITCH_CAPTURE_SHIFT);
    switch_reassigned |= (gpio_read_input(FUTURE_PROOF_PORT, FUTURE_PROOF_1_PIN)        << FUTURE_PROOF_1_SHIFT);
    switch_reassigned |= (gpio_read_input(FUTURE_PROOF_PORT, FUTURE_PROOF_2_PIN)        << FUTURE_PROOF_2_SHIFT);
    switch_reassigned |= (gpio_read_input(FUTURE_PROOF_PORT, FUTURE_PROOF_3_PIN)        << FUTURE_PROOF_3_SHIFT);

    // Apply an inversion mask to the active-low switches
    return (switch_reassigned ^ SWITCH_MASK);
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SWITCH_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(SWITCH_TIMER);

    // Read the switches into the vport image so we can model the switches as a physical port with a custom bit ordering
    switch_vport.image          = switch_shift_assign();

    // TODO: Remove this once all limit switches are in play
    switch_vport.image          = (switch_vport.image & ~(LIMIT_Z_MASK || LIMIT_Y_MASK || LIMIT_X_MASK));

    // Update the switch transition information
    p_switches->current_inputs  = switch_vport.image;
    p_switches->edges           = (p_switches->current_inputs ^ p_switches->previous_inputs);
    p_switches->pos_transitions = (p_switches->current_inputs & p_switches->edges);
    p_switches->neg_transitions = ((~p_switches->current_inputs) & p_switches->edges);
    p_switches->previous_inputs = p_switches->current_inputs;
}

/* End buttons.c */
