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
#define BITS64_MASK(shift)                  ((uint64_t) (1ULL << shift))
#define SQUARE_CENTER_TO_CENTER             (48)    // mm
#define SQUARE_X_INITIAL                    (-115)  // mm
#define SQUARE_Y_INITIAL                    (30)    // mm
#define CAPTURE_X                           (-20)
#define CAPTURE_Y                           (SQUARE_Y_INITIAL + 5*SQUARE_CENTER_TO_CENTER)
#define HOMING_X_BACKOFF                    (-6)    // mm
#define HOMING_Y_BACKOFF                    (6)     // mm
#define HOMING_Z_BACKOFF                    (0)     // mm
#define HOMING_X_VELOCITY                   (1)     // mm
#define HOMING_Y_VELOCITY                   (1)     // mm
#define HOMING_Z_VELOCITY                   (0)     // mm
#define HOMING_DELAY_MS                     (100)   // ms

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

// Bitfield for peripheral imaging (accessing RAM)
typedef struct {
    volatile unsigned bit_0  : 1;
    volatile unsigned bit_1  : 1;
    volatile unsigned bit_2  : 1;
    volatile unsigned bit_3  : 1;
    volatile unsigned bit_4  : 1;
    volatile unsigned bit_5  : 1;
    volatile unsigned bit_6  : 1;
    volatile unsigned bit_7  : 1;
    volatile unsigned bit_8  : 1;
    volatile unsigned bit_9  : 1;
    volatile unsigned bit_10 : 1;
    volatile unsigned bit_11 : 1;
    volatile unsigned bit_12 : 1;
    volatile unsigned bit_13 : 1;
    volatile unsigned bit_14 : 1;
    volatile unsigned bit_15 : 1;
    volatile unsigned bit_16 : 1;
    volatile unsigned bit_17 : 1;
    volatile unsigned bit_18 : 1;
    volatile unsigned bit_19 : 1;
    volatile unsigned bit_20 : 1;
    volatile unsigned bit_21 : 1;
    volatile unsigned bit_22 : 1;
    volatile unsigned bit_23 : 1;
    volatile unsigned bit_24 : 1;
    volatile unsigned bit_25 : 1;
    volatile unsigned bit_26 : 1;
    volatile unsigned bit_27 : 1;
    volatile unsigned bit_28 : 1;
    volatile unsigned bit_29 : 1;
    volatile unsigned bit_30 : 1;
    volatile unsigned bit_31 : 1;
    volatile unsigned bit_32 : 1;
    volatile unsigned bit_33 : 1;
    volatile unsigned bit_34 : 1;
    volatile unsigned bit_35 : 1;
    volatile unsigned bit_36 : 1;
    volatile unsigned bit_37 : 1;
    volatile unsigned bit_38 : 1;
    volatile unsigned bit_39 : 1;
    volatile unsigned bit_40 : 1;
    volatile unsigned bit_41 : 1;
    volatile unsigned bit_42 : 1;
    volatile unsigned bit_43 : 1;
    volatile unsigned bit_44 : 1;
    volatile unsigned bit_45 : 1;
    volatile unsigned bit_46 : 1;
    volatile unsigned bit_47 : 1;
    volatile unsigned bit_48 : 1;
    volatile unsigned bit_49 : 1;
    volatile unsigned bit_50 : 1;
    volatile unsigned bit_51 : 1;
    volatile unsigned bit_52 : 1;
    volatile unsigned bit_53 : 1;
    volatile unsigned bit_54 : 1;
    volatile unsigned bit_55 : 1;
    volatile unsigned bit_56 : 1;
    volatile unsigned bit_57 : 1;
    volatile unsigned bit_58 : 1;
    volatile unsigned bit_59 : 1;
    volatile unsigned bit_60 : 1;
    volatile unsigned bit_61 : 1;
    volatile unsigned bit_62 : 1;
    volatile unsigned bit_63 : 1;
} utils_bits64_t;

// Tracks move type
typedef enum chess_move_type {
    IDLE, MOVE, CAPTURE, PROMOTION, EN_PASSENT, CASTLING
} chess_move_type;

// Translates file on the board to distance in mm
typedef enum chess_file_t {
    A = SQUARE_X_INITIAL - 7*SQUARE_CENTER_TO_CENTER,
    B = SQUARE_X_INITIAL - 6*SQUARE_CENTER_TO_CENTER,
    C = SQUARE_X_INITIAL - 5*SQUARE_CENTER_TO_CENTER,
    D = SQUARE_X_INITIAL - 4*SQUARE_CENTER_TO_CENTER,
    E = SQUARE_X_INITIAL - 3*SQUARE_CENTER_TO_CENTER,
    F = SQUARE_X_INITIAL - 2*SQUARE_CENTER_TO_CENTER,
    G = SQUARE_X_INITIAL - 1*SQUARE_CENTER_TO_CENTER,
    H = SQUARE_X_INITIAL,
    CAPTURE_FILE = CAPTURE_X,
    FILE_ERROR = 0
} chess_file_t;

// Translates rank on the board to distance in mm
typedef enum chess_rank_t {
    FIRST   = SQUARE_Y_INITIAL,
    SECOND  = SQUARE_Y_INITIAL + 1*SQUARE_CENTER_TO_CENTER,
    THIRD   = SQUARE_Y_INITIAL + 2*SQUARE_CENTER_TO_CENTER,
    FOURTH  = SQUARE_Y_INITIAL + 3*SQUARE_CENTER_TO_CENTER,
    FIFTH   = SQUARE_Y_INITIAL + 4*SQUARE_CENTER_TO_CENTER,
    SIXTH   = SQUARE_Y_INITIAL + 5*SQUARE_CENTER_TO_CENTER,
    SEVENTH = SQUARE_Y_INITIAL + 6*SQUARE_CENTER_TO_CENTER,
    EIGHTH  = SQUARE_Y_INITIAL + 7*SQUARE_CENTER_TO_CENTER,
    CAPTURE_RANK = CAPTURE_Y,
    RANK_ERROR = 0
} chess_rank_t;

// Translates piece type to distance to lower rack in mm
//  TODO: FIll in once measured
typedef enum chess_piece_t {
    KING   = 1,
    QUEEN  = 2,
    ROOK   = 3,
    BISHOP = 4,
    KNIGHT = 5,
    PAWN   = 6,
    EMPTY  = 0
} chess_piece_t;

// Tracks whether a peripheral is enabled or disabled
typedef enum {
    disabled, enabled
} peripheral_state_t;

// Virtual ports for peripheral imaging
typedef union utils_vport8_t {
    volatile uint8_t image;
    utils_bits8_t bitfield;
} utils_vport8_t;

typedef union utils_vport64_t {
    volatile uint64_t image;
    utils_bits64_t bitfield;
} utils_vport64_t;

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

// Math utilities
uint16_t utils_bound(uint16_t value, uint16_t lower_bound, uint16_t upper_bound);


// Chess utilities
chess_file_t utils_byte_to_file(uint8_t byte);
chess_rank_t utils_byte_to_rank(uint8_t byte);
chess_move_type utils_byte_to_move_type(uint8_t byte);

// Interrupt utils
void utils_set_nvic(uint8_t interrupt_num, uint8_t priority);

// Command queue utils
void utils_empty_function();

#endif /* UTILS_H_ */
