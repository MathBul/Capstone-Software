/*
 * delay.c
 *
 *  Created on: Oct 30, 2022
 *      Author: EJ
 */

#include "delay.h"

static uint32_t count;


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

void delay_entry(command_t* command)
{
    delay_command_t* p_delay_command = (delay_command_t*) command;
    count = p_delay_command->time_ms * 5; // Timer counts down in 0.0002 s

    // Enable the timer
    clock_set_timer_period(DELAY_TIMER, DELAY_PERIOD);
    clock_resume_timer(DELAY_TIMER);
}

void delay_action(command_t* command)
{
    return; // All action is in the interrupt
}

void delay_exit(command_t* command)
{
    return; // Nothing
}

bool delay_is_done(command_t* command)
{
    return count == 0;
}

__interrupt void DELAY_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(DELAY_TIMER);

    // Decrement the clock
    count--;

    if (count == 0)
    {
        clock_pause_timer(DELAY_TIMER);
    }

}

