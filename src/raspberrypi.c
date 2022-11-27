/**
 * @file raspberrypi.c
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "raspberrypi.h"

// Private functions
static void rpi_checksum(char *data, uint8_t size);

/**
 * @brief Initialize the Raspberry Pi UART Tx and Rx lines
 */
void rpi_init(void)
{
    uart_init(RPI_UART_CHANNEL);
}

/**
 * @brief Uses UART to send data from the MSP432 to the Raspberry Pi
 *  TODO: Replace with call to uart_out_string? Remove delay?
 *
 * @param data Character buffer to be sent
 * @param size Number of characters to transmit (unless a null-terminator is reacher)
 * @return Whether transmission was successful
 */
bool rpi_transmit(char* data, uint8_t size)
{
    bool status = true;

    // Write one byte at a time
    int i = 0;
    for (i = 0; (i < size) && (status) && (data[i] != '\0'); i++)
    {
        status &= uart_out_byte(RPI_UART_CHANNEL, (uint8_t) data[i]);
        utils_delay(70000);
    }

    return status;
}

/**
 * @brief Uses UART to read data from the the Raspberry Pi to the MSP432
 *
 * @param data Character buffer to be sent
 * @param size Number of characters to transmit (unless a null-terminator is reacher)
 * @return Whether the read was successful
 */
bool rpi_receive(char *data, uint8_t size)
{
    return uart_read_string(RPI_UART_CHANNEL, data, size);
}

/**
 * @brief Attaches a checksum to a UART message
 * 
 * @param data Character buffer to be checksummed
 * @param size Number of characters to checksum, should be (msg_length - 2)
 */
static void rpi_checksum(char *data, uint8_t size)
{
    char check_bytes[2];
    
    utils_fl16_data_to_checkbytes((uint8_t *) data, size, check_bytes);
    data[size]   = check_bytes[0];
    data[size+1] = check_bytes[1];
}

/**
 * @brief Send a RESET instruction from the MSP432 to the Raspberry Pi
 *
 * @return Whether the transmission was successful
 */
bool rpi_transmit_reset(void)
{
    uint8_t msg_length = 4;
    char message[msg_length];

    // Build the message
    message[0] = START_BYTE;
    message[1] = RESET_INSTR_AND_LEN;
    rpi_checksum(message, msg_length-2);

    // Transmit
    return rpi_transmit(message, msg_length);
}

/**
 * @brief Send a START_W or START_B instruction from the MSP432 to the Raspberry Pi
 * 
 * @param color The color the human player is playing as
 * @return Whether the transmission was successful
 */
bool rpi_transmit_start(char color)
{
    uint8_t msg_length = 4;
    char message[msg_length];

    // Build the message
    message[0] = START_BYTE;
    message[1] = START_W_INSTR_AND_LEN;
    if (color == 'B')
    {
        message[1] = START_B_INSTR_AND_LEN;
    }
    rpi_checksum(message, msg_length-2);

    // Transmit
    return rpi_transmit(message, msg_length);
}

/**
 * @brief Send a HUMAN_MOVE instruction from the MSP432 to the Raspberry Pi
 * 
 * @param move A 4-5 character array containing the human's move in UCI notation, padded with '_'
 * @return Whether the transmission was successful
 */
bool rpi_transmit_human_move(char move[5])
{
    uint8_t msg_length = 4;
    char message[msg_length];

    // Build the message
    message[0] = START_BYTE;
    message[1] = HUMAN_MOVE_INSTR_AND_LEN;
    message[2] = move[0];
    message[3] = move[1];
    message[4] = move[2];
    message[5] = move[3];
    message[6] = move[4];
    rpi_checksum(message, msg_length-2);

    // Transmit
    return rpi_transmit(message, msg_length);
}

/**
 * @brief Send an ACK signal to the Raspberry Pi
 *
 * @return Whether the transmission was successful
 */
bool rpi_transmit_ack(void)
{
    return uart_out_byte(RPI_UART_CHANNEL, (uint8_t) ACK_BYTE);
}

/**
 * @brief Clears the Tx and Rx fifos for RPi communication
 */
void rpi_reset_uart(void)
{
    uart_reset(RPI_UART_CHANNEL);
}

/**
 * @brief UCI defines castling as one of four possible king moves. Returns the corresponding rook move
 *
 * @param king_move The castle move that describes how the king moves
 * @return The corresponding rook move
 */
chess_move_t rpi_castle_get_rook_move(chess_move_t *p_king_move)
{
    chess_rank_t dest_rank = p_king_move->dest_rank;
    chess_file_t dest_file = p_king_move->dest_file;
    chess_move_t rook_move;

    // Castling can happen in rank 1 or 8
    if (dest_rank == FIRST)         // White side castle
    {
        // White queen-side castle. King's move: e1c1 <=> Rook's move: a1d1
        if (dest_file == C)
        {
            rook_move.source_file = A;
            rook_move.source_rank = FIRST;
            rook_move.dest_file   = D;
            rook_move.dest_rank   = FIRST;
            rook_move.move_type   = MOVE;
        }
        // White king-side castle. King's move: e1g1 <=> Rook's move: h1f1
        else if (dest_file == G)
        {
            rook_move.source_file = H;
            rook_move.source_rank = FIRST;
            rook_move.dest_file   = F;
            rook_move.dest_rank   = FIRST;
            rook_move.move_type   = MOVE;
        }
    }
    else if (dest_rank == EIGHTH)   // Black side castle
    {
        // Black queen-side castle. King's move: e8c8 <=> Rook's move: a8d8
        if (dest_file == C)
        {
            rook_move.source_file = A;
            rook_move.source_rank = EIGHTH;
            rook_move.dest_file   = D;
            rook_move.dest_rank   = EIGHTH;
            rook_move.move_type   = MOVE;
        }
        // Black king-side castle. King's move: e8g8 <=> Rook's move: h8f8
        else if (dest_file == G)
        {
            rook_move.source_file = H;
            rook_move.source_rank = EIGHTH;
            rook_move.dest_file   = F;
            rook_move.dest_rank   = EIGHTH;
            rook_move.move_type   = MOVE;
        }
    }
    else
    {
        // Error, something went wrong
        return *p_king_move;
    }

    return rook_move;
}



/* End raspberrypi.c */
