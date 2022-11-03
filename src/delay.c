/**
 * @file delay.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a waiting command
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "delay.h"

static uint32_t count;

/**
 * @brief Dynamically allocates a delay command
 * 
 * @param time_ms The amount of time to wait in milliseconds (ms)
 * @return delay_command_t* The command
 */
delay_command_t* delay_build_command(uint16_t time_ms)
{
    delay_command_t* p_command = (delay_command_t*)malloc(sizeof(delay_command_t));

    // The command structure
    p_command->command.p_entry = &delay_entry;
    p_command->command.p_action = &delay_action;
    p_command->command.p_exit = &delay_exit;
    p_command->command.p_is_done = &delay_is_done;

    // Our data
    p_command->time_ms = time_ms;

    return p_command;
}

/**
 * @brief Run once before the action function
 * 
 * @param command The delay command from the command queue
 */
void delay_entry(command_t* command)
{
    delay_command_t* p_delay_command = (delay_command_t*) command;
    count = p_delay_command->time_ms * 5; // Timer counts down in 0.0002 s

    // Enable the timer
    clock_set_timer_period(DELAY_TIMER, DELAY_PERIOD);
    clock_start_timer(DELAY_TIMER);
}

/**
 * @brief Run repeatedly from the command queue
 * 
 * @param command The delay command from the command queue
 */
void delay_action(command_t* command)
{
    return; // All action is in the interrupt
}

/**
 * @brief Run once after the action function
 * 
 * @param command The delay command from the command queue
 */
void delay_exit(command_t* command)
{
    return; // Nothing
}

/**
 * @brief Determines when the action function is complete
 * 
 * @param command The delay command from the command queue
 * @return true If the time_ms has elapsed
 * @return false 
 */
bool delay_is_done(command_t* command)
{
    return count == 0;
}

/**
 * @brief Decrements a counter to effectivly do a busy wait
 * 
 * @return __interrupt 
 */
__interrupt void DELAY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(DELAY_TIMER);

    // Decrement the clock
    count--;

    if (count == 0)
    {
        clock_stop_timer(DELAY_TIMER);
    }

}

