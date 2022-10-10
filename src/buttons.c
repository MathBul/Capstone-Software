/**
 * @file buttons.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral buttons
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "buttons.h"

// TODO: Add event-based interrupts

// Declare the buttons
button_t buttons[NUMBER_OF_BUTTONS];
static button_t* button_start = &buttons[0];
static button_t* button_reset = &buttons[1];
static button_t* button_home  = &buttons[2];

/**
 * @brief Initialize all buttons
 */
void buttons_init()
{
    /* Button 1 (start) */
    gpio_set_as_input(BUTTON_1_PORT, BUTTON_1_PIN);

    // Configure the button struct
    button_start->button_port = BUTTON_1_PORT;
    button_start->button_pin  = BUTTON_1_PIN;
    

    /* Button 2 (reset) */
    gpio_set_as_input(BUTTON_2_PORT, BUTTON_2_PIN);

    // Configure the button struct
    button_reset->button_port = BUTTON_2_PORT;
    button_reset->button_pin  = BUTTON_2_PIN;
    

    /* Button 3 (home) */
    gpio_set_as_input(BUTTON_3_PORT, BUTTON_3_PIN);

    // Configure the button struct
    button_home->button_port = BUTTON_3_PORT;
    button_home->button_pin  = BUTTON_3_PIN;
}

/**
 * @brief Read the start button
 * 
 * @return button_state_t Whether the button is engaged or disengaged
 */
button_state_t button_read_start()
{
    uint8_t button_data = gpio_read_input(button_start->button_port, button_start->button_pin);
    if (button_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/**
 * @brief Read the reset button
 * 
 * @return button_state_t Whether the button is engaged or disengaged
 */
button_state_t button_reset_start()
{
    uint8_t button_data = gpio_read_input(button_reset->button_port, button_reset->button_pin);
    if (button_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/**
 * @brief Read the home button
 * 
 * @return button_state_t Whether the button is engaged or disengaged
 */
button_state_t button_read_home()
{
    uint8_t button_data = gpio_read_input(button_home->button_port, button_home->button_pin);
    if (button_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/* End buttons.c */
