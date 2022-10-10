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

/**
 * @brief Turn the move LED on (opponent should move, robot is waiting)
 */
void leds_all_on()
{
    int i = 0;
    led_t* base_led = &leds[0];
    for (i = 0; i < NUMBER_OF_LEDS; i++)
    {
        gpio_set_output_low((base_led+i)->enable_port, (base_led+i)->enable_pin);
    }
}

/**
 * @brief Turn the move LED off (opponent should move, robot is waiting)
 */
void leds_all_off()
{
    int i = 0;
    led_t* base_led = &leds[0];
    for (i = 0; i < NUMBER_OF_LEDS; i++)
    {
        gpio_set_output_high((base_led+i)->enable_port, (base_led+i)->enable_pin);
    }
}

/* End leds.c */
