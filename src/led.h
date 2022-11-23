/**
 * @file led.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral LEDs
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef LED_H_
#define LED_H_

// Note on LED translations:
//  - RED    <=> Error 
//  - BLUE   <=> Robot is moving
//  - GREEN  <=> Human is moving
//  - SIGNAL <=> System operational

#include "msp.h"
#include "command_queue.h"
#include "gpio.h"
#include <stdint.h>
#include <stdlib.h>

// General LED macros
#define NUMBER_OF_LEDS                      (4)

// Game status LED macros
#define RGB_RED_PORT                        (GPIOC)
#define RGB_RED_PIN                         (GPIO_PIN_4)
#define RGB_BLUE_PORT                       (GPIOC)
#define RGB_BLUE_PIN                        (GPIO_PIN_5)
#define RGB_GREEN_PORT                      (GPIOE)
#define RGB_GREEN_PIN                       (GPIO_PIN_4)

// System status LED macros
#define SIGNAL_LIGHT_PORT                   (GPION)
#define SIGNAL_LIGHT_PIN                    (GPIO_PIN_3)

// LED indicator type
typedef enum {
    led_error, 
    led_robot_move, 
    led_human_move, 
    led_system_status
} led_indicator_t;

// LED structure
typedef struct {
    GPIO_Type* enable_port;       // Port for controlling the LED
    uint8_t    enable_pin;        // Pin for controlling the LED
} led_t;

// Public functions
void leds_init();
void led_on(led_indicator_t indicator);
void led_off(led_indicator_t indicator);
void led_all_on();
void led_all_off();

#endif /* LED_H_ */
