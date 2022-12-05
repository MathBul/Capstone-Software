/**
 * @file led.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral LEDs
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "led.h"

// Private functions
static void led_enable(led_t* p_led);
static void led_disable(led_t* p_led);

// Declare the LEDs
led_t leds[NUMBER_OF_LEDS];
static led_t* p_led_error         = &leds[0];
static led_t* p_led_robot_move    = &leds[1];
static led_t* p_led_human_move    = &leds[2];
static led_t* p_led_system_status = &leds[3];

/**
 * @brief Initialize all LEDs
 */
void led_init(void)
{
    // Game status LEDs
    gpio_set_as_output(RGB_RED_PORT, RGB_RED_PIN);
    gpio_set_output_low(RGB_RED_PORT, RGB_RED_PIN);
    p_led_error->enable_port = RGB_RED_PORT;
    p_led_error->enable_pin  = RGB_RED_PIN;

    gpio_set_as_output(RGB_BLUE_PORT, RGB_BLUE_PIN);
    gpio_set_output_low(RGB_BLUE_PORT, RGB_BLUE_PIN);
    p_led_robot_move->enable_port = RGB_BLUE_PORT;
    p_led_robot_move->enable_pin  = RGB_BLUE_PIN;
    
    gpio_set_as_output(RGB_GREEN_PORT, RGB_GREEN_PIN);
    gpio_set_output_low(RGB_GREEN_PORT, RGB_GREEN_PIN);
    p_led_human_move->enable_port = RGB_GREEN_PORT;
    p_led_human_move->enable_pin  = RGB_GREEN_PIN;

    // System status LEDs
    gpio_set_as_output(SIGNAL_LIGHT_PORT, SIGNAL_LIGHT_PIN);
    gpio_set_output_low(SIGNAL_LIGHT_PORT, SIGNAL_LIGHT_PIN);
    p_led_system_status->enable_port = SIGNAL_LIGHT_PORT;
    p_led_system_status->enable_pin  = SIGNAL_LIGHT_PIN;
}

/**
 * @brief Helper function to enable an LED
 * 
 * @param p_led The LED to enable
 */
static void led_enable(led_t* p_led)
{
    gpio_set_output_high(p_led->enable_port, p_led->enable_pin);
}

/**
 * @brief Helper function to disable an LED
 * 
 * @param p_led The LED to disable
 */
static void led_disable(led_t* p_led)
{
    gpio_set_output_low(p_led->enable_port, p_led->enable_pin);
}

/**
 * @brief Turn on the specified LED
 */
void led_on(led_indicator_t indicator)
{
    switch (indicator)
    {
        case led_error:
            led_enable(p_led_error);
        break;

        case led_robot_move:
            led_enable(p_led_robot_move);
        break;

        case led_human_move:
            led_enable(p_led_human_move);
        break;

        case led_system_status:
            led_enable(p_led_system_status);
        break;

        default:
            // Invalid LED indicator provided, do nothing
        break;
    }
}

/**
 * @brief Turn off the specified LED
 */
void led_off(led_indicator_t indicator)
{
    switch (indicator)
    {
        case led_error:
            led_disable(p_led_error);
        break;

        case led_robot_move:
            led_disable(p_led_robot_move);
        break;

        case led_human_move:
            led_disable(p_led_human_move);
        break;

        case led_system_status:
            led_disable(p_led_system_status);
        break;

        default:
            // Invalid LED indicator provided, do nothing
        break;
    }
}

/**
 * @brief Turn on all LEDs
 */
void led_all_on(void)
{
    int i = 0;
    for (i = 0; i < NUMBER_OF_LEDS; i++)
    {
        led_enable(&leds[i]);
    }
}

/**
 * @brief Turn off all LEDs
 */
void led_all_off(void)
{
    int i = 0;
    for (i = 0; i < NUMBER_OF_LEDS; i++)
    {
        led_disable(&leds[i]);
    }
}

/* End led.c */
