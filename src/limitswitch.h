/**
 * @file limitswitch.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for peripheral limit switches
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef LIMITSWITCH_H_
#define LIMITSWITCH_H_

#include "msp.h"
#include "gpio.h"
#include <stdint.h>

// General button defines
#define NUMBER_OF_LIMIT_SWITCHES            (3)

// Button 1 (start)
#define LIMIT_SWITCH_1_PORT                 (GPIOD)         // TODO: Update when known
#define LIMIT_SWITCH_1_PIN                  (GPIO_PIN_0)    // TODO: Update when known

// Button 2 (reset)
#define LIMIT_SWITCH_2_PORT                 (GPIOD)         // TODO: Update when known
#define LIMIT_SWITCH_2_PIN                  (GPIO_PIN_1)    // TODO: Update when known

// Button 3 (home)
#define LIMIT_SWITCH_3_PORT                 (GPIOD)         // TODO: Update when known
#define LIMIT_SWITCH_3_PIN                  (GPIO_PIN_2)    // TODO: Update when known

// Button state
typedef enum {
    engaged, disengaged
} limitswitch_state_t;

// Button structure
typedef struct {
    GPIO_Type* switch_port;     // Port for the limit switch
    uint8_t    switch_pin;      // Pin for the limit switch
} limitswitch_t;

// Public functions
void limitswitch_init();
limitswitch_state_t limitswitch_read_x();
limitswitch_state_t limitswitch_read_y();
limitswitch_state_t limitswitch_read_z();

#endif /* LIMITSWITCH_H_ */
