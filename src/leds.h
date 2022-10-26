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

// Assumed LED translations:
//  RED   <=> Error     (illegal move, etc.)
//  BLUE  <=> Wait      (opponent should wait, robot is moving)
//  GREEN <=> Move      (opponent should move, robot is waiting)

#include "msp.h"
#include "command_queue.h"
#include "gpio.h"
#include <stdint.h>

// RGB LED 1 defines
#define NUMBER_OF_LEDS                      (3)
#define LED_1_RED_PORT                      (GPIOD)         // TODO: Update when known
#define LED_1_RED_PIN                       (GPIO_PIN_0)    // TODO: Update when known
#define LED_1_BLUE_PORT                     (GPIOD)         // TODO: Update when known
#define LED_1_BLUE_PIN                      (GPIO_PIN_1)    // TODO: Update when known
#define LED_1_GREEN_PORT                    (GPIOD)         // TODO: Update when known
#define LED_1_GREEN_PIN                     (GPIO_PIN_2)    // TODO: Update when known

// LED structure
typedef struct {
    GPIO_Type* enable_port;       // Port for the LED
    uint8_t    enable_pin;        // Pin for the LED, active low
} led_t;

// Public functions
void leds_init();
void leds_error_on();
void leds_error_off();
void leds_wait_on();
void leds_wait_off();
void leds_move_on();
void leds_move_off();
void leds_all_on();
void leds_all_off();

// Command functions
void leds_entry(command_t* command);
void leds_action(command_t* command);
void leds_exit(command_t* command);
bool leds_is_done(command_t* command);

#endif /* LEDS_H_ */
