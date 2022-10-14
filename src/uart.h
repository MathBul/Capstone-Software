/**
 * @file uart.h
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for the UART communications on the MSP432e401Y
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef UART_H 
#define UART_H

#include "msp.h"
#include <stdint.h>
#include "uart_fifo.h"

// These are all the UART channels on the msp432e401y
#define UART_CHANNEL_0          (0)
#define UART_CHANNEL_1          (1)
#define UART_CHANNEL_2          (2)
#define UART_CHANNEL_3          (3)
#define UART_CHANNEL_4          (4)
#define UART_CHANNEL_5          (5)
#define UART_CHANNEL_6          (6)
#define UART_CHANNEL_7          (7)

// Start byte
#define START_BYTE              (0x0A)

// Individual Instruction IDs (used for recognizing instructions received from the RPi)
#define RESET_INSTR             (0x00)
#define START_W_INSTR           (0x01)
#define START_B_INSTR           (0x02)
#define HUMAN_MOVE_INSTR        (0x03)
#define ROBOT_MOVE_INSTR        (0x04)
#define GAME_STATUS_INSTR       (0x05)
#define ILLEGAL_MOVE_INSTR      (0x06)

// Instruction and operand length bytes (used for transmitting to RPi)
#define RESET_INSTR_AND_LEN             (0x00)
#define START_W_INSTR_AND_LEN           (0x10)
#define START_B_INSTR_AND_LEN           (0x20)
#define HUMAN_MOVE_INSTR_AND_LEN        (0x35)
#define ROBOT_MOVE_INSTR_AND_LEN        (0x45)
#define GAME_STATUS_INSTR_AND_LEN       (0x51)
#define ILLEGAL_MOVE_INSTR_AND_LEN      (0x60)

// Full Instructions/Operations
#define RESET                   (0x0A00)           // Reset a terminated game
#define START_W                 (0x0A10)           // Start signal if human plays white (goes first)
#define START_B                 (0x0A20)           // Start signal if human plays black (goes second)
#define HUMAN_MOVE              (0x0A350000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define ROBOT_MOVE              (0x0A450000000000) // 5 operand bytes for UCI representation of move (fill in trailing zeroes with move)
#define GAME_ONGOING            (0x0A5101)         // Declare the game has not ended
#define GAME_CHECKMATE          (0x0A5102)         // Declare the game has ended to checkmate
#define GAME_STALEMATE          (0x0A5103)         // Declare the game has ended to stalemate
#define ILLEGAL_MOVE            (0x0A60)           // Declare the human has made an illegal move

// UART function declarations
void uart_init(uint8_t uart_channel);
bool uart_out_byte(uint8_t uart_channel, uint8_t byte);
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte);


#endif /* UART_H */
