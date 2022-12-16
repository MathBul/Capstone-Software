/**
 * @file main.c
 * @author Eli Jelesko (ebj5hec@virginia.edu), Nick Cooney (npc4crc@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Main method using command queue approach
 * @version 1.0
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 *
 * This runs on a MSP432e401Y microcontroller, and the accompanying PCB is designed to attach to the launch pad version of the chip.
 * To deploy this onto the chip, first set the heap and stack size to 4096 in your linker settings before flashing.
 */

#include "msp.h"
#include "gantry.h"

int main(void)
{
    // System level initialization
    command_queue_init();
    gantry_init();

#if defined(GANTRY_DEBUG) || defined(STEPPER_DEBUG)
    // Add specific commands to the queue
    gantry_home();
    command_queue_push((command_t*) stepper_build_chess_xy_command(H, FIRST, 1, 1));
    command_queue_push((command_t*) delay_build_command(1000));
    command_queue_push((command_t*) stepper_build_chess_z_command(PAWN, 1));
    command_queue_push((command_t*) delay_build_command(1000));
    command_queue_push((command_t*) stepper_build_chess_z_command(HOME_PIECE, 1));

#else
    // Play chess
    command_queue_push((command_t*) gantry_reset_build_command());

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
                // Check for a system fault (E-stop, etc.) or reset
                if (sys_fault)
                {
                    // In the case of a fault, force a hard fault
                    command_t* p_bad_command = NULL;
                    p_bad_command->p_entry(p_bad_command);
                    break;
                }
                else if (sys_reset || sys_limit)
                {
                    // In the case of a reset, skip actions until the the homing or reset button clears
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
