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
static void led_toggle(led_t* p_led);
static void led_start_flash();
static void led_stop_flash();

// Declare the LEDs
led_t leds[NUMBER_OF_LEDS];
static led_t* p_led_red   = &leds[0];
static led_t* p_led_blue  = &leds[1];
static led_t* p_led_green = &leds[2];

// Private variables
static bool led_red_status   = false;
static bool led_green_status = false;
static bool led_blue_status  = false;

/**
 * @brief Initialize all LEDs
 */
void led_init(void)
{
    // Game status LEDs
    gpio_set_as_output(RGB_RED_PORT, RGB_RED_PIN);
    gpio_set_output_low(RGB_RED_PORT, RGB_RED_PIN);
    p_led_red->enable_port = RGB_RED_PORT;
    p_led_red->enable_pin  = RGB_RED_PIN;

    gpio_set_as_output(RGB_BLUE_PORT, RGB_BLUE_PIN);
    gpio_set_output_low(RGB_BLUE_PORT, RGB_BLUE_PIN);
    p_led_blue->enable_port = RGB_BLUE_PORT;
    p_led_blue->enable_pin  = RGB_BLUE_PIN;
    
    gpio_set_as_output(RGB_GREEN_PORT, RGB_GREEN_PIN);
    gpio_set_output_low(RGB_GREEN_PORT, RGB_GREEN_PIN);
    p_led_green->enable_port = RGB_GREEN_PORT;
    p_led_green->enable_pin  = RGB_GREEN_PIN;
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
 * @param flashing Whether the LED should flash or be solid
 */
static void led_disable(led_t* p_led)
{
    gpio_set_output_low(p_led->enable_port, p_led->enable_pin);
}

/**
 * @brief Helper function to toggle an LED
 *
 * @param p_led The LED to toggle
 */
static void led_toggle(led_t* p_led)
{
    gpio_set_output_toggle(p_led->enable_port, p_led->enable_pin);
}

/**
 * Helper function to start the LED flashing
 */
static void led_start_flash()
{
    clock_start_timer(LED_TIMER);
}

/**
 * Helper function to stop the LED flashing
 */
static void led_stop_flash()
{
    clock_stop_timer(LED_TIMER);
}

/**
 * @brief Turn on the specified LED
 */
void led_mode(led_indicator_t indicator)
{
    switch (indicator)
    {
        case LED_ERROR:
            led_stop_flash();
            led_enable(p_led_red);
            led_disable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = true;
            led_green_status = false;
            led_blue_status  = false;
        break;

        case LED_ROBOT_MOVE:
            led_stop_flash();
            led_disable(p_led_red);
            led_disable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = false;
            led_green_status = false;
            led_blue_status  = true;
        break;

        case LED_ROBOT_WIN:
            led_disable(p_led_red);
            led_disable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = false;
            led_green_status = false;
            led_blue_status  = true;
            led_start_flash();
        break;

        case LED_HUMAN_MOVE:
            led_stop_flash();
            led_disable(p_led_red);
            led_enable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = false;
            led_green_status = true;
            led_blue_status  = false;
        break;

        case LED_HUMAN_WIN:
            led_enable(p_led_red);
            led_enable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = true;
            led_green_status = true;
            led_blue_status  = true;
            led_start_flash();
        break;

        case LED_WAITING_FOR_MSG:
            led_stop_flash();
            led_enable(p_led_red);
            led_disable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = true;
            led_green_status = false;
            led_blue_status  = true;
        break;

        case LED_STALEMATE:
            led_enable(p_led_red);
            led_enable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = true;
            led_green_status = true;
            led_blue_status  = true;
            led_start_flash();
        break;

        case LED_OFF:
            led_stop_flash();
            led_disable(p_led_red);
            led_disable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = false;
            led_green_status = false;
            led_blue_status  = false;
        break;

        case LED_SCANNING_ERROR_WHITE:
            led_enable(p_led_red);
            led_enable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = true;
            led_green_status = true;
            led_blue_status  = false;
            led_start_flash();
        break;

        case LED_SCANNING_ERROR_BLACK:
            led_disable(p_led_red);
            led_enable(p_led_green);
            led_enable(p_led_blue);

            led_red_status   = false;
            led_green_status = true;
            led_blue_status  = true;
            led_start_flash();
        break;

        case LED_CAPTURE:
            led_disable(p_led_red);
            led_enable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = false;
            led_green_status = true;
            led_blue_status  = false;
            led_start_flash();
        break;

        case LED_ILLEGAL:
            led_enable(p_led_red);
            led_disable(p_led_green);
            led_disable(p_led_blue);

            led_red_status   = true;
            led_green_status = false;
            led_blue_status  = false;
            led_start_flash();
        break;

        default:
            // Invalid LED indicator provided, do nothing
        break;
    }
}

/**
 * @brief Interrupt handler for the LED module
 */
__interrupt void LED_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(LED_TIMER);

    // Toggle any LED that is on
    if (led_red_status)
    {
        led_toggle(p_led_red);
    }
    if (led_green_status)
    {
        led_toggle(p_led_green);
    }
    if (led_blue_status)
    {
        led_toggle(p_led_blue);
    }
}

/* End led.c */
