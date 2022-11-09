// Project-specific source code
#include "gantry.h"

// Standard includes necessary for base functionality
#include "msp.h"

//#define UART_DEBUG
#define MOTION_PROFILING
#define COMMAND_QUEUE

// Flag that gets set in the utils module when there is a system fault
extern bool utils_sys_fault;

int main(void)
{

#ifdef COMMAND_QUEUE
    // Probably need to allocate enough memory for the largest command struct
    command_t* p_current_command;

    // System level initialization
    gantry_init();

#ifdef MOTION_PROFILING
    command_queue_push((command_t*)stepper_build_rel_command(100, 0, 0, 1, 0, 0));
#else
    // Add commands to the queue
    gantry_start();
#endif


    // Main program flow
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
                // Check for a system fault (e-stop, etc.)
                if (utils_sys_fault)
                {
                    break;  // TODO: Break both loops?
                }
                p_current_command->p_action(p_current_command);
            }

            // Run the exit function
            p_current_command->p_exit(p_current_command);
            // Free the command
            free(p_current_command);
        }
    }
#endif

    #ifdef UART_DEBUG

    gantry_init();

    // Read whatever comes in to the message string.
    while (1)
    {
        uart_out_string(UART_CHANNEL_0, "Hello world!\n", 14);
    }
    #endif


}

/* End main.c */
