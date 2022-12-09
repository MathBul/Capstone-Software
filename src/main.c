/**
 * @file main.c
 * @author Eli Jelesko (ebj5hec@virginia.edu), Nick Cooney (npc4crc@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Main method using command queue approach
 * @version 1.0
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 */

#include "msp.h"
#include "gantry.h"

int main(void)
{
    // System level initialization
    command_queue_init();
    gantry_init();

    // Add commands to the queue
#ifdef SENSOR_NETWORK_DEBUG
    // No exiting main
    while(1){}

#elif defined(UART_DEBUG)
    while (1)
    {
        uart_out_string(UART_CHANNEL_0, "Hello world!\n", 14);
    }

#elif defined(STEPPER_DEBUG) || defined(GANTRY_DEBUG)
    // Test a chess movement
    command_queue_push((command_t*) delay_build_command(2000));
//    gantry_home();
//    command_queue_push((command_t*) gantry_robot_build_command());

    // Test a switch
    gpio_set_as_output(SWITCH_TEST_PORT, SWITCH_TEST_PIN);
    gpio_set_output_high(SWITCH_TEST_PORT, SWITCH_TEST_PIN);

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
                    // In the case of a fault, indefinitely delay until sys_fault has been reset
                    break;
                }
                p_current_command->p_action(p_current_command);
            }

            // Run the exit function
            p_current_command->p_exit(p_current_command);

            // Free the command memory
            free(p_current_command);
        }

#ifdef GANTRY_DEBUG
        switch_test(E_STOP_MASK);
#endif
    }
}

/* End main.c */
