/**
 * @file buttons.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral buttons
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "msp.h"
#include "gpio.h"
#include <stdint.h>

// General button defines
#define NUMBER_OF_BUTTONS                   (3)

// Button 1 (start)
#define BUTTON_1_PORT                        (GPIOD)         // TODO: Update when known
#define BUTTON_1_PIN                         (GPIO_PIN_0)    // TODO: Update when known

// Button 2 (reset)
#define BUTTON_2_PORT                        (GPIOD)         // TODO: Update when known
#define BUTTON_2_PIN                         (GPIO_PIN_1)    // TODO: Update when known

// Button 3 (home)
#define BUTTON_3_PORT                        (GPIOD)         // TODO: Update when known
#define BUTTON_3_PIN                         (GPIO_PIN_2)    // TODO: Update when known

// Button state
typedef enum {
    engaged, disengaged
} button_state_t;

// Button structure
typedef struct {
    GPIO_Type* button_port;     // Port for the button
    uint8_t    button_pin;      // Pin for the button
} button_t;

// Public functions
void buttons_init();
button_state_t button_read_start();
button_state_t button_read_reset();
button_state_t button_read_home();

#endif /* BUTTONS_H_ */
