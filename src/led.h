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

#include "msp.h"
#include "clock.h"
#include "gpio.h"
#include <stdint.h>
#include <stdlib.h>

// General LED macros
#define NUMBER_OF_LEDS                      (3)
#define LED_TIMER                           (TIMER6)
#define LED_HANDLER                         (TIMER6A_IRQHandler)

// Game status LED macros
#define RGB_RED_PORT                        (GPIOC)
#define RGB_RED_PIN                         (GPIO_PIN_4)
#define RGB_BLUE_PORT                       (GPIOC)
#define RGB_BLUE_PIN                        (GPIO_PIN_5)
#define RGB_GREEN_PORT                      (GPIOE)
#define RGB_GREEN_PIN                       (GPIO_PIN_4)

// LED indicator type
typedef enum {
    LED_ERROR,
    LED_ROBOT_MOVE,
    LED_HUMAN_MOVE,
    LED_WAITING_FOR_MSG,
    LED_ROBOT_WIN,
    LED_HUMAN_WIN,
    LED_STALEMATE,
    LED_OFF,
    LED_SCANNING_ERROR,
    LED_CAPTURE,
} led_indicator_t;

// LED structure
typedef struct {
    GPIO_Type* enable_port;       // Port for controlling the LED
    uint8_t    enable_pin;        // Pin for controlling the LED
} led_t;

// Public functions
void led_init(void);
void led_mode(led_indicator_t indicator);;

#endif /* LED_H_ */
