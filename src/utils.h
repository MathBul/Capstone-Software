/**
 * @file utils.h
 * @author Nick Cooney (npc4crc@virginia.edu), Eli Jelesko (ebj5hec@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Utility functions that may be used by more than one module
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef UTILS_H_
#define UTILS_H_

// Debug mode select
#define PERIPHERALS_ENABLED         // Enable electromagent and sensor network
//#define GANTRY_DEBUG                // Run specific gantry commands
//#define STEPPER_DEBUG               // Debug motion profiling

// Game mode select (define at most one at a time)
//#define THREE_PARTY_MODE            // User sends moves to MSP, which sends moves to RPi, which sends moves back
#define FINAL_IMPLEMENTATION_MODE   // Final implementation w/ board reading

// Notes on vports: 
//  - A virtual port (vport) is a means of accessing a physical port via imaging and a bitfield
//  - An image is a snapshot of the port's data register
//  - The bitfield allows for access to specific pins
//  - Presumably, each peripheral will have a vport and a shift_assign() function to map the vport's
//      bitfield to a local ordering. This allows for only minor refactoring with changes in the peripheral's GPIO
//  - For an example use case, see switch.c/switch.h

#include "msp.h"
#include "command_queue.h"
#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// General utility macros
#define BITS8_MASK(shift)                   ((uint8_t) (1 << shift))
#define BITS16_MASK(shift)                  ((uint16_t) (0x0001 << shift))
#define BITS64_MASK(shift)                  ((uint64_t) (1ULL << shift))

// Chess-specific macros
#define SQUARE_CENTER_TO_CENTER             (48)        // mm
#define SQUARE_X_INITIAL                    (-134)      // mm
#define SQUARE_Y_INITIAL                    (26)        // mm
#define CAPTURE_X                           (-20)       // mm
#define CAPTURE_Y                           (SQUARE_Y_INITIAL + 5*SQUARE_CENTER_TO_CENTER)
#define QUEEN_X                             (-26)       // mm
#define QUEEN_Y                             (SQUARE_Y_INITIAL + 1*SQUARE_CENTER_TO_CENTER)
#define PIECE_HEIGHT_OFFSET                 (44)

// Motor-specific macros
#define HOMING_X_BACKOFF                    (-6)        // mm
#define HOMING_Y_BACKOFF                    (6)         // mm
#define HOMING_Z_BACKOFF                    (-6)        // mm
#define HOMING_X_VELOCITY                   (1)         // mm
#define HOMING_Y_VELOCITY                   (1)         // mm
#define HOMING_Z_VELOCITY                   (1)         // mm
#define HOMING_DELAY_MS                     (100)       // ms

// Shared flags
extern bool sys_fault;
extern bool sys_reset;

// Bitfield for peripheral imaging (accessing 8-bit volatile memory)
typedef struct utils_bits8_tutils_bits8_t {
    volatile unsigned bit_0 : 1;
    volatile unsigned bit_1 : 1;
    volatile unsigned bit_2 : 1;
    volatile unsigned bit_3 : 1;
    volatile unsigned bit_4 : 1;
    volatile unsigned bit_5 : 1;
    volatile unsigned bit_6 : 1;
    volatile unsigned bit_7 : 1;
} utils_bits8_t;

// Bitfield for peripheral imaging (accessing 16-bit volatile memory)
typedef struct utils_bits16_t {
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
} utils_bits16_t;

// 8-bit virtual port for peripheral imaging
typedef union utils_vport8_t {
    volatile uint8_t image;
    utils_bits8_t bitfield;
} utils_vport8_t;

// 16-bit virtual port for peripheral imaging
typedef union utils_vport16_t {
    volatile uint16_t image;
    utils_bits16_t bitfield;
} utils_vport16_t;

// Tracks move type
typedef enum chess_move_type_t {
    IDLE, 
    MOVE, 
    CAPTURE, 
    PROMOTION,
    CAPTURE_PROMOTION,
    EN_PASSENT, 
    CASTLING,
} chess_move_type_t;

// Translates file on the board to distance in mm
typedef enum chess_file_t {
    A            = SQUARE_X_INITIAL + 2,
    B            = SQUARE_X_INITIAL - 1*SQUARE_CENTER_TO_CENTER + 5,
    C            = SQUARE_X_INITIAL - 2*SQUARE_CENTER_TO_CENTER + 7,
    D            = SQUARE_X_INITIAL - 3*SQUARE_CENTER_TO_CENTER + 8,
    E            = SQUARE_X_INITIAL - 4*SQUARE_CENTER_TO_CENTER + 9,
    F            = SQUARE_X_INITIAL - 5*SQUARE_CENTER_TO_CENTER + 9,
    G            = SQUARE_X_INITIAL - 6*SQUARE_CENTER_TO_CENTER + 10,
    H            = SQUARE_X_INITIAL - 7*SQUARE_CENTER_TO_CENTER + 11,
    CAPTURE_FILE = CAPTURE_X,
    QUEEN_FILE   = QUEEN_X,
    HOME_FILE    = HOMING_X_BACKOFF,
    FILE_ERROR   = 1,
} chess_file_t;

// Translates rank on the board to distance in mm
typedef enum chess_rank_t {
    FIRST        = SQUARE_Y_INITIAL + 7*SQUARE_CENTER_TO_CENTER - 8,
    SECOND       = SQUARE_Y_INITIAL + 6*SQUARE_CENTER_TO_CENTER - 5,
    THIRD        = SQUARE_Y_INITIAL + 5*SQUARE_CENTER_TO_CENTER - 3,
    FOURTH       = SQUARE_Y_INITIAL + 4*SQUARE_CENTER_TO_CENTER - 3,
    FIFTH        = SQUARE_Y_INITIAL + 3*SQUARE_CENTER_TO_CENTER - 2,
    SIXTH        = SQUARE_Y_INITIAL + 2*SQUARE_CENTER_TO_CENTER - 1,
    SEVENTH      = SQUARE_Y_INITIAL + 1*SQUARE_CENTER_TO_CENTER,
    EIGHTH       = SQUARE_Y_INITIAL + 2,
    CAPTURE_RANK = CAPTURE_Y,
    QUEEN_RANK   = QUEEN_Y,
    HOME_RANK    = HOMING_Y_BACKOFF,
    RANK_ERROR   = 1,
} chess_rank_t;

// Translates piece type to distance to lower rack in mm
typedef enum chess_piece_t {
    KING        = -55 - PIECE_HEIGHT_OFFSET,
    QUEEN       = -68 - PIECE_HEIGHT_OFFSET,
    ROOK        = -91 - PIECE_HEIGHT_OFFSET,
    BISHOP      = -77 - PIECE_HEIGHT_OFFSET,
    KNIGHT      = -84 - PIECE_HEIGHT_OFFSET,
    PAWN        = -90 - PIECE_HEIGHT_OFFSET,
    HOME_PIECE  = HOMING_Z_BACKOFF - 4,
    EMPTY_PIECE = 1,
} chess_piece_t;

// Tracks whether a peripheral is enabled or disabled
typedef enum peripheral_state_t {
    disabled, enabled
} peripheral_state_t;

// GPIO, interrupt, and empty utils
void utils_gpio_clock_enable(GPIO_Type* port);
void utils_uart_clock_enable(uint8_t uart_channel);
void utils_timer_clock_enable(TIMER0_Type* timer);
void utils_delay(uint32_t ticks);
void utils_set_nvic(uint8_t interrupt_num, uint8_t priority);
void utils_empty_function(command_t* command);

// Math and bit manipulation utils
uint16_t utils_bound(uint16_t value, uint16_t lower_bound, uint16_t upper_bound);
uint8_t utils_bits8_get_lsb_shift(uint8_t mask);
uint8_t utils_bits8_remask(uint8_t byte, uint8_t original_mask, uint8_t new_mask);

// Fletcher-16 checksum utilsutils
uint16_t utils_fl16_data_to_checksum(uint8_t *data, int count);
void utils_fl16_checksum_to_checkbytes(uint16_t checksum, char check_bytes[2]);
void utils_fl16_data_to_checkbytes(uint8_t *data, int count, char check_bytes[2]);
bool utils_validate_transmission(uint8_t *data, int count, char check_bytes[2]);

// Chess utils
uint8_t utils_tile_to_index(chess_file_t file, chess_rank_t rank);
chess_rank_t utils_index_to_rank(uint8_t index);
chess_file_t utils_index_to_file(uint8_t index);
chess_file_t utils_byte_to_file(uint8_t byte);
chess_rank_t utils_byte_to_rank(uint8_t byte);
chess_move_type_t utils_byte_to_move_type(uint8_t byte);
chess_piece_t utils_byte_to_piece_type(uint8_t byte);
int32_t utils_calculate_offset(int32_t pos_x, int32_t pos_y, int32_t pos_z);

#endif /* UTILS_H_ */
