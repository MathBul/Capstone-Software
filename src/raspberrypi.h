/**
 * @file raspberrypi.h
 * @author Nick Cooney (npc4crc@virginia.edu) and Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef RASPBERRYPI_H_
#define RASPBERRYPI_H_

#include "msp.h"
#include "command_queue.h"
#include "gpio.h"
#include "uart.h"
#include "utils.h"
#include <stdint.h>
#include <stdbool.h>

// Mode switch (define one at a time)
//#define USER_MODE // (user, through UART0 terminal, sends moves to MSP directly)
//#define THREE_PARTY_MODE // (user sends moves to MSP, which sends moves to RPi, which sends moves back)
#define FINAL_IMPLEMENTATION_MODE // (ideally, final implementation w/board reading)

// General Raspberry Pi defines
#ifdef USER_MODE
#   define RPI_UART_CHANNEL               (UART_CHANNEL_0)
#elif defined(THREE_PARTY_MODE)
#   define RPI_UART_CHANNEL               (UART_CHANNEL_3)
#   define USER_CHANNEL                   (UART_CHANNEL_0)
#else
#   define RPI_UART_CHANNEL               (UART_CHANNEL_3)
#endif

/*
 * All UART instruction defines
 * UART instructions are defined as:
 *   - 1 start byte (0x0A)
 *   - 1 byte containing the instruction ID (4 bits)
 *     and the operand length in bytes (4 bits)
 *   - 0 - 5 bytes containing the operand
 *   - 2 bytes containing the check bytes for the instruction
 */

// Start byte + ACK signal
#define START_BYTE                          (0x0A)
#define ACK_BYTE                            (0x0F)

// Individual instruction IDs
#define RESET_INSTR                         (0x00)
#define START_W_INSTR                       (0x01)
#define START_B_INSTR                       (0x02)
#define HUMAN_MOVE_INSTR                    (0x03)
#define ROBOT_MOVE_INSTR                    (0x04)
#define ILLEGAL_MOVE_INSTR                  (0x05)

// Instruction and operand length bytes
#define RESET_INSTR_AND_LEN                 (0x00)
#define START_W_INSTR_AND_LEN               (0x10)
#define START_B_INSTR_AND_LEN               (0x20)
#define HUMAN_MOVE_INSTR_AND_LEN            (0x35)
#define ROBOT_MOVE_INSTR_AND_LEN            (0x46)
#define ILLEGAL_MOVE_INSTR_AND_LEN          (0x50)

// Full Instructions/Operations
#define RESET                               (0x0A00)             // Reset a terminated game
#define START_W                             (0x0A10)             // Start signal if human plays white (goes first)
#define START_B                             (0x0A20)             // Start signal if human plays black (goes second)
#define HUMAN_MOVE                          (0x0A35000000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define ROBOT_MOVE                          (0x0A46000000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define ILLEGAL_MOVE                        (0x0A50)             // Declare the human has made an illegal move

// Game status codes
#define GAME_ONGOING                        (0x01)
#define GAME_CHECKMATE                      (0x02)
#define GAME_STALEMATE                      (0x03)

// Information from the PI for making a chess move
// Use '\0' for undefined file and 0 for undefined rank
typedef struct chess_move_t {
    chess_file_t source_file;        // The letter
    chess_rank_t source_rank;        // The number
    chess_file_t dest_file;
    chess_rank_t dest_rank;
    chess_move_type move_type;
} chess_move_t;

typedef enum game_status_t {
    ONGOING,
    HUMAN_WIN,
    ROBOT_WIN,
    STALEMATE
} game_status_t;

// Public Raspberry Pi functions
void rpi_init(void);
bool rpi_transmit(char* data, uint8_t num_chars);
bool rpi_receive(char *data, uint8_t num_chars);
void rpi_reset_uart(void);

// Raspberry Pi Instruction functions
bool rpi_transmit_reset(void);
bool rpi_transmit_start(char color);
bool rpi_transmit_human_move(char *move);
bool rpi_transmit_ack(void);
chess_move_t rpi_get_castle_rook_move(chess_move_t *king_move);

#endif /* RASPBERRYPI_H_ */
