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

//#define ALL_PORTS

// Private functions
static uint8_t switch_shift_assign(uint8_t port_raw);

// Declare the switches
#ifdef ALL_PORTS
static switches_t switches;
static switches_t* p_switches = &switches;
#else
static switch_state_t switches;
static switch_state_t* p_switches = (&switches);
#endif

/**
 * @brief Initialize all buttons
 */
void switch_init()
{
     // Configure GPIO for all switches
    gpio_set_as_input(SWITCH_PORT, BUTTON_PIN_MASK);
    gpio_set_as_input(SWITCH_PORT, LIMIT_PIN_MASK);
    gpio_set_as_input(SWITCH_PORT, ROCKER_PIN_MASK);
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
     switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_RESET_PIN,       BUTTON_START_RESET);
     switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_ESTOP_PIN,       BUTTON_ESTOP);
     switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_HOME_PIN,        BUTTON_HOME);
     switch_reassigned |= utils_bits8_remask(port_raw, BUTTON_END_TURN_PIN,    BUTTON_END_TURN);
     switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_X_PIN,            LIMIT_X);
     switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_Y_PIN,            LIMIT_Y);
     switch_reassigned |= utils_bits8_remask(port_raw, LIMIT_Z_PIN,            LIMIT_Z);
     switch_reassigned |= utils_bits8_remask(port_raw, ROCKER_COLOR_PIN,       ROCKER_COLOR);
     return (switch_reassigned ^ SWITCH_MASK);
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SWITCH_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(SWITCH_TIMER);


#ifdef ALL_PORTS
    // Eli's Theory
    switches.image.alpha |= (((uint8_t)GPIOA->DATA) << 0);
    switches.image.alpha |= (((uint8_t)GPIOB->DATA) << 8);
    switches.image.alpha |= (((uint8_t)GPIOC->DATA) << 16);
    switches.image.alpha |= (((uint8_t)GPIOD->DATA) << 24);
    switches.image.alpha |= (((uint8_t)GPIOE->DATA) << 32);
    switches.image.alpha |= (((uint8_t)GPIOF->DATA) << 40);
    switches.image.alpha |= (((uint8_t)GPIOG->DATA) << 48);
    switches.image.alpha |= (((uint8_t)GPIOH->DATA) << 56);
    switches.image.beta |= (((uint8_t)GPIOJ->DATA) << 0);
    switches.image.beta |= (((uint8_t)GPIOK->DATA) << 8);
    switches.image.beta |= (((uint8_t)GPIOL->DATA) << 16);
    switches.image.beta |= (((uint8_t)GPIOM->DATA) << 24);
    switches.image.beta |= (((uint8_t)GPION->DATA) << 32);
    switches.image.beta |= (((uint8_t)GPIOP->DATA) << 40);
    switches.image.beta |= (((uint8_t)GPIOQ->DATA) << 48);


#else
    // Read the switches into the vport image. Lets us model a physical port with a custom bit ordering
    uint8_t switches_raw        = gpio_read_input(SWITCH_PORT, SWITCH_MASK);
    switch_vport.image          = switch_shift_assign(switches_raw);

    // TODO: Remove this once all limit switches are in play
    switch_vport.image          = (switch_vport.image & ~(LIMIT_Y | LIMIT_Z));

    // Update the switch transition information
    p_switches->current_inputs  = switch_vport.image;
    p_switches->edges           = (p_switches->current_inputs ^ p_switches->previous_inputs);
    p_switches->pos_transitions = (p_switches->current_inputs & p_switches->edges);
    p_switches->neg_transitions = ((~p_switches->current_inputs) & p_switches->edges);
    p_switches->previous_inputs = p_switches->current_inputs;
#endif



}

/* End buttons.c */
