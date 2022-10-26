/**
 * @file utils.h
 * @author Nick Cooney (npc4crc@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Utility functions that may be used by more than one module
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef UTILS_H_
#define UTILS_H_

// Notes on vports: 
//  - A virtual port (vport) is a means of accessing a physical port via imaging and a bitfield
//  - An image is a snapshot of the port's data register
//  - The bitfield allows for access to specific pins
//  - Presumably, each peripheral will have a vport and a shift_assign() function to map the vport's
//      bitfield to a local ordering. This allows for minor refactoring with changes in the peripheral's GPIO
//  - For an example use case, see switch.c/switch.h

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// General utility macros
#define BITS8_MASK(shift)                   ((uint8_t) (1 << shift))

// Bitfield for peripheral imaging (accessing RAM)
typedef struct {
    volatile unsigned bit_0 : 1;
    volatile unsigned bit_1 : 1;
    volatile unsigned bit_2 : 1;
    volatile unsigned bit_3 : 1;
    volatile unsigned bit_4 : 1;
    volatile unsigned bit_5 : 1;
    volatile unsigned bit_6 : 1;
    volatile unsigned bit_7 : 1;
} utils_bits8_t;

// Virtual port for peripheral imaging
typedef union utils_vport_t {
    volatile uint8_t image;
    utils_bits8_t bitfield;
} utils_vport_t;

// GPIO utility functions
void utils_gpio_clock_enable(GPIO_Type* port);
void utils_delay(uint32_t ticks);

// Bit manipulation utility functions
uint8_t utils_bits8_mask_to_shift(uint8_t mask);
uint8_t utils_bits8_remask(uint8_t byte, uint8_t original_mask, uint8_t new_mask);

// Fletcher checksum function
uint16_t fletcher_16(uint8_t *data, int count);

#endif /* UTILS_H_ */
