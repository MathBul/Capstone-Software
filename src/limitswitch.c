/**
 * @file limitswitch.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral limit switches
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "limitswitch.h"

// TODO: Add event-based interrupts

// Declare the limit switches
limitswitch_t switches[NUMBER_OF_LIMIT_SWITCHES];
static limitswitch_t* limitswitch_x = &switches[0];
static limitswitch_t* limitswitch_y = &switches[1];
static limitswitch_t* limitswitch_z  = &switches[2];

/**
 * @brief Initialize all buttons
 */
void limitswitch_init()
{
    /* Switch 1 (x-axis) */
    gpio_set_as_input(LIMIT_SWITCH_1_PORT, LIMIT_SWITCH_1_PIN);

    // Configure the button struct
    limitswitch_x->switch_port = LIMIT_SWITCH_1_PORT;
    limitswitch_x->switch_pin  = LIMIT_SWITCH_1_PIN;
    

    /* Switch 2 (y-axis) */
    gpio_set_as_input(LIMIT_SWITCH_2_PORT, LIMIT_SWITCH_2_PIN);

    // Configure the button struct
    limitswitch_y->switch_port = LIMIT_SWITCH_2_PORT;
    limitswitch_y->switch_pin  = LIMIT_SWITCH_2_PIN;
    

    /* Switch 3 (z-axis) */
    gpio_set_as_input(LIMIT_SWITCH_3_PORT, LIMIT_SWITCH_3_PIN);

    // Configure the button struct
    limitswitch_z->switch_port = LIMIT_SWITCH_3_PORT;
    limitswitch_z->switch_pin  = LIMIT_SWITCH_3_PIN;
}

/**
 * @brief Read the x-axis switch
 * 
 * @return limitswitch_state_t Whether the button is engaged or disengaged
 */
limitswitch_state_t limitswitch_read_x()
{
    uint8_t switch_data = gpio_read_input(limitswitch_x->switch_port, limitswitch_x->switch_pin);
    if (switch_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/**
 * @brief Read the y-axis switch
 * 
 * @return limitswitch_state_t Whether the button is engaged or disengaged
 */
limitswitch_state_t limitswitch_read_y()
{
    uint8_t switch_data = gpio_read_input(limitswitch_y->switch_port, limitswitch_y->switch_pin);
    if (switch_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/**
 * @brief Read the z-axis switch
 * 
 * @return limitswitch_state_t Whether the button is engaged or disengaged
 */
limitswitch_state_t limitswitch_read_z()
{
    uint8_t switch_data = gpio_read_input(limitswitch_z->switch_port, limitswitch_z->switch_pin);
    if (switch_data == 0)
    {
        return engaged;
    }
    return disengaged;
}

/* End limitswitch.c */
