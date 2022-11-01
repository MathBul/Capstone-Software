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
#define SQUARE_CENTER_TO_CENTER             (48) //mm
#define SQUARE_X_INITIAL                    (-100)
#define SQUARE_Y_INITIAL                    (30)


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

typedef enum chess_move_type {
    IDLE, MOVE, CAPTURE, PROMOTION, EN_PASSENT, CASTLING
} chess_move_type;



typedef enum chess_file_t {
    A = SQUARE_X_INITIAL - 7*SQUARE_CENTER_TO_CENTER,
    B = SQUARE_X_INITIAL - 6*SQUARE_CENTER_TO_CENTER,
    C = SQUARE_X_INITIAL - 5*SQUARE_CENTER_TO_CENTER,
    D = SQUARE_X_INITIAL - 4*SQUARE_CENTER_TO_CENTER,
    E = SQUARE_X_INITIAL - 3*SQUARE_CENTER_TO_CENTER,
    F = SQUARE_X_INITIAL - 2*SQUARE_CENTER_TO_CENTER,
    G = SQUARE_X_INITIAL - 1*SQUARE_CENTER_TO_CENTER,
    H = SQUARE_X_INITIAL,
    FILE_ERROR = 0
} chess_file_t;

typedef enum chess_rank_t {
    FIRST   = SQUARE_Y_INITIAL,
    SECOND  = SQUARE_Y_INITIAL + 1*SQUARE_CENTER_TO_CENTER,
    THIRD   = SQUARE_Y_INITIAL + 2*SQUARE_CENTER_TO_CENTER,
    FOURTH  = SQUARE_Y_INITIAL + 3*SQUARE_CENTER_TO_CENTER,
    FIFTH   = SQUARE_Y_INITIAL + 4*SQUARE_CENTER_TO_CENTER,
    SIXTH   = SQUARE_Y_INITIAL + 5*SQUARE_CENTER_TO_CENTER,
    SEVENTH = SQUARE_Y_INITIAL + 6*SQUARE_CENTER_TO_CENTER,
    EIGHTH  = SQUARE_Y_INITIAL + 7*SQUARE_CENTER_TO_CENTER,
    RANK_ERROR = 0
} chess_rank_t;

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

// Fletcher-16 checksum functions
uint16_t utils_fl16_data_to_csum(uint8_t *data, int count);
char* utils_fl16_csum_to_cbytes(uint16_t checksum, char check_bytes[2]);
char* utils_fl16_data_to_cbytes(uint8_t *data, int count, char check_bytes[2]);
bool utils_validate_transmission(uint8_t *data, int count, char check_bytes[2]);

// Math utility
uint16_t utils_bound(uint16_t value, uint16_t lower_bound, uint16_t upper_bound);

// Chess
chess_file_t utils_byte_to_file(uint8_t byte);
chess_rank_t utils_byte_to_rank(uint8_t byte);
chess_move_type utils_byte_to_move_type(uint8_t byte);


// Interrupts
void utils_set_nvic(uint8_t interrupt_num);

#endif /* UTILS_H_ */
