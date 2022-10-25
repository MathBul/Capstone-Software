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

/**
 * @brief Interprets an instruction from the RPi, stored in the gantry_command struct
 * 
 * @param gantry_command The command containing the instruction
 */
void gantry_interpret(gantry_command_t* gantry_command)
{
    // TODO: Switch statement to interpret the instruction read from the Pi
    //  Store values in the gantry command struct for access in the gantry_exit() function
}

/* Command Functions */

/**
 * @brief Prepares the gantry for this command
 * 
 * @param command 
 */
void gantry_entry(command_t* command)
{
    return;
}

/**
 * @brief Attempts to read from the RPi until data is sent
 * 
 * @param command The gantry command being acted on
 */
void gantry_action(command_t* command)
{
    // =============================== TODO ===============================
    // Wait for turn_end flag in buttons.c
    // Read scan of board from sensor_network.c
    // Interpret board with chessboard.c
    // Send UART with raspberrypi.c
    // Read UART with raspberrypi.c
}

/**
 * @brief Interprets the RPi's move, and adds the appropriate commands to the queue
 * 
 * @param command The gantry command exiting
 */
void gantry_exit(command_t* command)
{
    gantry_command_t* p_gantry_command = (gantry_command_t*) command;
    
    // Interpret the move
    gantry_interpret(p_gantry_command);

    // lc = led_build_command()
    // command_queue_push(lc)
    
    // TODO: Fill in this switch statement
    switch (p_gantry_command->instruction_type) {
        case 'M':   // Move
            // sc = stepper_build_command()
            // command_queue_push(sc)
            // ec = electromagnet_build_command()
            // command_queue_push(ec)
            // sc = stepper_build_command()
            // command_queue_push(sc)
            // ec = electromagnet_build_command()
            // command_queue_push(ec)
            // sc = stepper_build_command()
            // command_queue_push(sc)
            break;  
        case 'P':   // Promotion
            // TODO
            break;
        
        case 'C':   // Capture
            // TODO 
            break;

        case 'c':   // Castling
            // TODO
            break;

        case 'E':   // En passante
            // TODO
            break;

        default:
            // TODO
            break;
    }

    // lc = led_build_command()
    // command_queue_push(lc)
    // snc = sensor_network_build_command()
    // command_queue_push(snc)
}

bool gantry_is_done(command_t* command)
{

}




/* Interrupt service routines for all modules */

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SWITCH_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_ifg_raw(SWITTCH_TIMER, timer_a);
    
    // Read the switches
    uint8_t switch_data = switch_read_all();

    // If a limit switch was pressed, disable the appropriate motor
    // if (switch_data & LIMIT_X_PIN)
    // {
    //     stepper_disable_motor(STEPPER_X_ID);
    // }
    // if (switch_data & LIMIT_Y_PIN)
    // {
    //     stepper_disable_motor(STEPPER_Y_ID);
    // }
    // if (switch_data & LIMIT_Z_PIN)
    // {
    //     stepper_disable_motor(STEPPER_Z_ID);
    // }

    // If the start/reset button was pressed, send the appropriate "new game" signal (Should use non-momentary...)
    if (switch_data & BUTTON_START_RESET)
    {
        // Reset the steppers
        //stepper_reset();

        // Reset the chess board
        chessboard_reset();

        // Reset the rpi
        rpi_transmit_reset();

        // Start the new game
        rpi_transmit_start(switch_get_color());
    }

    // If the home button was pressed, clear the queue and execute a homing command
    if (switch_data & BUTTON_HOME)
    {
        //stepper_reset();
    }
}

/* End gantry.c */
