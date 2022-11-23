/**
 * @file delay.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a waiting command
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 */

#include "delay.h"

// Busy wait counter
static uint32_t count;

/**
 * @brief Dynamically allocates a delay command
 * 
 * @param time_ms The amount of time to wait in milliseconds (ms)
 * @return The command
 */
delay_command_t* delay_build_command(uint16_t time_ms)
{
    // The thing to return
    delay_command_t* p_command = (delay_command_t*) malloc(sizeof(delay_command_t));

    // Functions
    p_command->command.p_entry   = &delay_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &delay_is_done;

    // Data
    p_command->time_ms = time_ms;

    return p_command;
}

/**
 * @brief Run once before the action function
 * 
 * @param command A delay command from the command queue
 */
void delay_entry(command_t* command)
{
    delay_command_t* p_delay_command = (delay_command_t*) command;

    // Interrupts execute every 1ms
    count = p_delay_command->time_ms;

    // Enable the timer
    clock_set_timer_period(DELAY_TIMER, DELAY_PERIOD);
    clock_start_timer(DELAY_TIMER);
}

/**
 * @brief Determines when the action function is complete
 * 
 * @param command A delay command from the command queue
 * @return Whether time_ms has elapsed
 */
bool delay_is_done(command_t* command)
{
    return (count == 0);
}

/**
 * @brief Decrements a counter to effectivly do a busy wait
 */
__interrupt void DELAY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(DELAY_TIMER);

    // Decrement the counter until it reaches 0
    count--;
    if (count == 0)
    {
        clock_stop_timer(DELAY_TIMER);
    }
}

/* End delay.c */
