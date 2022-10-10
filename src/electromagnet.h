/**
 * @file electromagnet.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions to control an electromagnet driven by a BTN8962TA chip
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef ELECTROMAGNET_H_
#define ELECTROMAGNET_H_

#include "msp.h"
#include "gpio.h"
#include <stdint.h>

// Electromagnet (EM)
#define NUMBER_OF_ELECTROMAGNETS            (1)
#define ELECTROMAGNET_INHIBIT_PORT          (GPIOD)         // TODO: Update when known
#define ELECTROMAGNET_INHIBIT_PIN           (GPIO_PIN_0)    // TODO: Update when known
#define ELECTROMAGNET_INPUT_PORT            (GPIOD)         // TODO: Update when known
#define ELECTROMAGNET_INPUT_PIN             (GPIO_PIN_1)    // TODO: Update when known

// Electromagnet structure
typedef struct {
    GPIO_Type* inhibit_port;        // Port used to enable/disable the EM
    uint8_t    inhibit_pin;         // Pin used to enable/disable the EM
    GPIO_Type* input_port;          // Port used to send high/low signals to the EM
    uint8_t    input_pin;           // Pin used to send high/low signals to the EM
} electromagnet_t;

// Public functions
void electromagnet_init();
void electromagnet_engage();
void electromagnet_disengage();

#endif /* ELECTROMAGNET_H_ */
