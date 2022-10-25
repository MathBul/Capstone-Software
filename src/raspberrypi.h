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

// General Raspberry Pi defines
#define RPI_UART_CHANNEL                    (UART_CHANNEL_3)

/*
 * All UART instruction defines
 * UART instructions are defined as:
 *   - 1 start byte (0x0A)
 *   - 1 byte containing the instruction ID (4 bits)
 *     and the operand length in bytes (4 bits)
 *   - 0 - 5 bytes containing the operand
 *   - 2 bytes containing the checksum for the instruction
 */

// Start byte
#define START_BYTE                          (0x0A)

// Individual instruction IDs
#define RESET_INSTR                         (0x00)
#define START_W_INSTR                       (0x01)
#define START_B_INSTR                       (0x02)
#define HUMAN_MOVE_INSTR                    (0x03)
#define ROBOT_MOVE_INSTR                    (0x04)
#define GAME_STATUS_INSTR                   (0x05)
#define ILLEGAL_MOVE_INSTR                  (0x06)

// Instruction and operand length bytes
#define RESET_INSTR_AND_LEN                 (0x00)
#define START_W_INSTR_AND_LEN               (0x10)
#define START_B_INSTR_AND_LEN               (0x20)
#define HUMAN_MOVE_INSTR_AND_LEN            (0x35)
#define ROBOT_MOVE_INSTR_AND_LEN            (0x45)
#define GAME_STATUS_INSTR_AND_LEN           (0x51)
#define ILLEGAL_MOVE_INSTR_AND_LEN          (0x60)

// Full Instructions/Operations
#define RESET                               (0x0A00)           // Reset a terminated game
#define START_W                             (0x0A10)           // Start signal if human plays white (goes first)
#define START_B                             (0x0A20)           // Start signal if human plays black (goes second)
#define HUMAN_MOVE                          (0x0A350000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define ROBOT_MOVE                          (0x0A450000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define GAME_ONGOING                        (0x0A5101)         // Declare the game has not ended
#define GAME_CHECKMATE                      (0x0A5102)         // Declare the game has ended to checkmate
#define GAME_STALEMATE                      (0x0A5103)         // Declare the game has ended to stalemate
#define ILLEGAL_MOVE                        (0x0A60)           // Declare the human has made an illegal move

/*
 * End UART instruction defines
 */

// Public Raspberry Pi functions
void rpi_init();
bool rpi_transmit(char* data, uint8_t num_chars);
bool rpi_receive(char *data, uint8_t num_chars);

// Raspberry Pi Instruction functions
void rpi_transmit_reset(void);
void rpi_transmit_start(char color);
void rpi_transmit_human_move(char *move);

// Command Functions
void rpi_entry(command_t* command);
void rpi_action(command_t* command);
void rpi_exit(command_t* command);
bool rpi_is_done(command_t* command);

#endif /* RASPBERRYPI_H_ */
