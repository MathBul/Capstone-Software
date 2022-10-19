/**
 * @file gantry.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Code to unite all other modules. All ISRs are implemented here
 * @version 0.1
 * @date 2022-10-19
 * 
 * @copyright Copyright (c) 2022
 */

#include "gantry.h"

/* Interrupt service routines for all modules */

/**
 * @brief Interrupt handler for the stepper module
 */
__interrupt void STEPPER_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt_raw(STEPPER_TIMER, timer_a);

    // Check if we are ready for a new position command
    if (stepper_arrived())
    {
        stepper_get_next_pos_commands();
    }

    // Either perform a transition, or disable the motors if no commands left
    stepper_take_action();
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SWITCH_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt_raw(SWITTCH_TIMER, timer_a);
    
    // Read the switches
    uint8_t switch_data = switch_read_all();

    // If a limit switch was pressed, disable the appropriate motor
    if (switch_data & LIMIT_X_PIN)
    {
        stepper_disable_motor(STEPPER_X_ID);
    }
    if (switch_data & LIMIT_Y_PIN)
    {
        stepper_disable_motor(STEPPER_Y_ID);
    }
    if (switch_data & LIMIT_Z_PIN)
    {
        stepper_disable_motor(STEPPER_Z_ID);
    }

    // If the start/reset button was pressed, send the appropriate "new game" signal (Should use non-momentary...)
    if (switch_data & BUTTON_START_RESET)
    {
        // Reset the steppers
        stepper_reset();

        // Reset the chess board
        chessboard_reset();

        // Reset the rpi
        rpi_transmit_RESET();

        // Start the new game
        rpi_transmit_start(switch_get_color());
    }

    // If the home button was pressed, clear the queue and execute a homing command
    if (switch_data & BUTTON_HOME)
    {
        stepper_reset();
    }
}

/* End gantry.c */
