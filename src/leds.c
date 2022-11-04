/**
 * @file leds.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral LEDs
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "leds.h"

// Private functions
static void leds_error_on();
static void leds_error_off();
static void leds_wait_on();
static void leds_wait_off();
static void leds_move_on();
static void leds_move_off();

// Declare the LEDs
led_t leds[NUMBER_OF_LEDS];
static led_t* led_error = &leds[0];
static led_t* led_wait  = &leds[1];
static led_t* led_move  = &leds[2];

/**
 * @brief Initialize all LEDs
 */
void leds_init()
{
    /* RGB LED 1, RED (stop) */
    // LED is active low
    gpio_set_as_output(LED_1_RED_PORT, LED_1_RED_PIN);
    gpio_set_output_high(LED_1_RED_PORT, LED_1_RED_PIN);
    
    // Configure the LED struct
    led_error->enable_port = LED_1_RED_PORT;
    led_error->enable_pin  = LED_1_RED_PIN;


    /* RGB LED 1, BLUE (wait) */
    // LED is active low
    gpio_set_as_output(LED_1_BLUE_PORT, LED_1_BLUE_PIN);
    gpio_set_output_high(LED_1_BLUE_PORT, LED_1_BLUE_PIN);
    
    // Configure the LED struct
    led_wait->enable_port = LED_1_BLUE_PORT;
    led_wait->enable_pin  = LED_1_BLUE_PIN;


    /* RGB LED 1, GREEN (move) */
    // LED is active low
    gpio_set_as_output(LED_1_GREEN_PORT, LED_1_GREEN_PIN);
    gpio_set_output_high(LED_1_GREEN_PORT, LED_1_GREEN_PIN);
    
    // Configure the LED struct
    led_wait->enable_port = LED_1_GREEN_PORT;
    led_wait->enable_pin  = LED_1_GREEN_PIN;
}

/**
 * @brief Turn the error LED on (illegal move, etc.)
 */
void leds_error_on()
{
    gpio_set_output_low(led_error->enable_port, led_error->enable_pin);
}

/**
 * @brief Turn the error LED off (illegal move, etc.)
 */
void leds_error_off()
{
    gpio_set_output_high(led_error->enable_port, led_error->enable_pin);
}

/**
 * @brief Turn the wait LED on (opponent should wait, robot is moving)
 */
void leds_wait_on()
{
    gpio_set_output_low(led_wait->enable_port, led_wait->enable_pin);
}

/**
 * @brief Turn the wait LED off (opponent should wait, robot is moving)
 */
void leds_wait_off()
{
    gpio_set_output_high(led_wait->enable_port, led_wait->enable_pin);
}

/**
 * @brief Turn the move LED on (opponent should move, robot is waiting)
 */
void leds_move_on()
{
    gpio_set_output_low(led_move->enable_port, led_move->enable_pin);
}

/**
 * @brief Turn the move LED off (opponent should move, robot is waiting)
 */
void leds_move_off()
{
    gpio_set_output_high(led_move->enable_port, led_move->enable_pin);
}

/* Command Functions */

/**
 * @brief Builds an LED command to turn on/off the desired indicator
 * 
 * @param indicator One of {ERROR, WAIT, MOVE}
 * @param desired_state One of {enabled, disabled}
 * @return Pointer to the command object
 */
led_command_t* leds_build_command(led_indicator_t indicator, peripheral_state_t desired_state)
{
    // The thing to return
    led_command_t* p_command = (led_command_t*) malloc(sizeof(led_command_t));

    // Functions
    p_command->command.p_entry   = &leds_entry;
    p_command->command.p_action  = &utils_empty_function;
    p_command->command.p_exit    = &utils_empty_function;
    p_command->command.p_is_done = &leds_is_done;

    // Data
    p_command->indicator_type = indicator;
    p_command->desired_state  = desired_state;

    return p_command;
}

/**
 * @brief Enables or disables the desired LED
 * 
 * @param command Pointer to the LED command
 */
void leds_entry(command_t* command)
{
    led_command_t* p_command = (led_command_t*) command;

    // Get the desired settings
    led_indicator_t indicator = p_command->indicator_type;
    peripheral_state_t desired_state = p_command->desired_state;

    // Enable or disable the ERROR LED if desired
    if (indicator == LED_ERROR)
    {
        if (desired_state == enabled)
        {
            leds_error_on();
        }
        else 
        {
            leds_error_off();
        }
    }

    // Enable or disable the ERROR LED if desired
    if (indicator == LED_WAIT)
    {
        if (desired_state == enabled)
        {
            leds_wait_on();
        }
        else 
        {
            leds_wait_off();
        }
    }

    // Enable or disable the MOVE LED if desired
    if (indicator == LED_MOVE)
    {
        if (desired_state == enabled)
        {
            leds_move_on();
        }
        else 
        {
            leds_move_off();
        }
    }
}

/**
 * @brief Always returns true since no action is done
 * 
 * @param command Pointer to the LED command
 * @return true Always
 */
bool leds_is_done(command_t* command)
{
    return true;
}

/* End leds.c */
