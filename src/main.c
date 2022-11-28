/**
 * @file main.c
 * @author Eli Jelesko (ebj5hec@virginia.edu), Nick Cooney (npc4crc@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Main method using command queue approach
 * @version 1.0
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 */

#define STEPPER_DEBUG


#include "msp.h"
#include "gantry.h"

// Debug mode select
// #define UART_DEBUG
// #define PERIPHERALS_ENABLED

// Game mode select (define at most one at a time)
// #define USER_MODE                 // User, through UART0 terminal, sends moves to MSP directly
// #define THREE_PARTY_MODE          // User sends moves to MSP, which sends moves to RPi, which sends moves back
// #define FINAL_IMPLEMENTATION_MODE // Ideally, final implementation w/board reading
int main(void)
{
    // System level initialization
    command_queue_init();
    gantry_init();

    // Add commands to the queue
#ifdef UART_DEBUG
    while (1)
    {
        uart_out_string(UART_CHANNEL_0, "Hello world!\n", 14);
    }
#elif defined(STEPPER_DEBUG)
    command_queue_push((command_t*) stepper_build_rel_command(0, 0, 40, 0, 0, 1));
    gpio_set_as_output(GPION, GPIO_PIN_0);
    gpio_set_output_high(GPION, GPIO_PIN_0);
#else
    gantry_reset();
#endif

    // Main program flow
    command_t* p_current_command;

    while (1)
    {

        // Run the entry function
        if (!command_queue_pop(&p_current_command))
        {
            // Something went wrong. Probably ran out of commands
        }
        else
        {
            p_current_command->p_entry(p_current_command);

            // Run the action function - is_done() determines when action is complete
            while (!p_current_command->p_is_done(p_current_command))
            {
                // Check for a system fault (E-stop, etc.)
                if (sys_fault)
                {
                    // TODO: Break both loops?
                    break;
                }
                p_current_command->p_action(p_current_command);
            }

            // Run the exit function
            p_current_command->p_exit(p_current_command);

            // Free the command memory
            free(p_current_command);
        }
    }
}

/* End main.c */
