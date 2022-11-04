/**
 * @file leds.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral LEDs
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef LEDS_H_
#define LEDS_H_

// Chosen LED translations:
//  RED   <=> ERROR     (illegal move, etc.)
//  BLUE  <=> WAIT      (opponent should wait, robot is moving)
//  GREEN <=> MOVE      (opponent should move, robot is waiting)

#include "msp.h"
#include "command_queue.h"
#include "gpio.h"
#include <stdint.h>
#include <stdlib.h>

// RGB LED 1 defines
#define NUMBER_OF_LEDS                      (3)
#define LED_1_RED_PORT                      (GPIOC)
#define LED_1_RED_PIN                       (GPIO_PIN_4)
#define LED_1_BLUE_PORT                     (GPIOC)
#define LED_1_BLUE_PIN                      (GPIO_PIN_5)
#define LED_1_GREEN_PORT                    (GPIOE)
#define LED_1_GREEN_PIN                     (GPIO_PIN_4)

// LED indicator type
typedef enum {
    LED_ERROR, LED_WAIT, LED_MOVE
} led_indicator_t;

// LED structure
typedef struct {
    GPIO_Type*      enable_port;       // Port for the LED
    uint8_t         enable_pin;        // Pin for the LED, active low
} led_t;

// LED command struct
typedef struct {
    command_t          command;
    led_indicator_t    indicator_type;
    peripheral_state_t desired_state;
} led_command_t;

// Public functions
void leds_init();

// Command functions
led_command_t* leds_build_command(led_indicator_t indicator, peripheral_state_t desired_state);
void leds_entry(command_t* command);
void leds_action(command_t* command);
void leds_exit(command_t* command);
bool leds_is_done(command_t* command);

#endif /* LEDS_H_ */
