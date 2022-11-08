// Project-specific source code
#include "gantry.h"

// Standard includes necessary for base functionality
#include "msp.h"

//#define UART_DEBUG
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

    // Add commands to the queue
    gantry_start();

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

    clock_sys_init();
    uart_init(UART_CHANNEL_0);

    // Read whatever comes in to the message string.
    while (1)
    {
        uart_out_string(UART_CHANNEL_0, "Hello world!\n");
    }
    #endif


}

/* End main.c */
