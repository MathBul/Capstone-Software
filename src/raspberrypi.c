/**
 * @file raspberrypi.c
 * @author Nick Cooney (npc4crc@virginia.edu) and Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "raspberrypi.h"

/**
 * @brief Initialize the Raspberry Pi UART Tx and Rx lines
 */
void rpi_init(void)
{
    uart_init(RPI_UART_CHANNEL);
}

/**
 * @brief Uses UART to send data from the MSP432 to the Raspberry Pi
 *
 * @param data An array of characters to send
 * @return True if successful; false otherwise
 */
bool rpi_transmit(char* data, uint8_t num_chars)
{
    bool status;
    uint8_t i = 0;
    
    for (i = 0; i < num_chars; i++)
    {
        status = uart_out_byte(RPI_UART_CHANNEL, (uint8_t) data[i]);

        utils_delay(70000);

        if (!status)
        {
            return false;
        }
    }
    // TODO: Checksum here?

    return true;
}

/**
 * @brief Uses UART to read data from the the Raspberry Pi to the MSP432
 *
 * @param data The character array to read the data into
 * @return True if successful; false otherwise
 */
bool rpi_receive(char *data, uint8_t num_chars)
{
    bool status;
    uint8_t i = 0;
    
    for (i = 0; i < num_chars; i++)
    {
        status = uart_read_byte(RPI_UART_CHANNEL, (uint8_t*) &data[i]);

        if (!status)
        {
            return false;
        }
    }

    return true;

}

/**
 * @brief Send a RESET instruction from the MSP432 to the Raspberry Pi
 *
 * @return true if the transmission was successful, false otherwise
 */
bool rpi_transmit_reset(void)
{
    char message[4];
    char check_bytes[2];

    message[0] = START_BYTE;
    message[1] = RESET_INSTR_AND_LEN;
    utils_fl16_data_to_cbytes((uint8_t *) message, 2, check_bytes);
    message[2] = check_bytes[0];
    message[3] = check_bytes[1];

    return rpi_transmit(message, 4);
}

/**
 * @brief Send a START_W or START_B instruction from the MSP432 to the Raspberry Pi
 * @param color A char representing the color the human player is playing as
 *
 * @return true if the transmission was successful, false otherwise
 */
bool rpi_transmit_start(char color)
{
    char message[4];
    char check_bytes[2];

    message[0] = START_BYTE;
    if (color == 'W')
    {
        message[1] = START_W_INSTR_AND_LEN;
    }
    else if (color == 'B')
    {
        message[1] = START_B_INSTR_AND_LEN;
    }
    else
    {
        message[1] = START_W_INSTR_AND_LEN;
    }
    utils_fl16_data_to_cbytes((uint8_t *) message, 2, check_bytes);
    message[2] = check_bytes[0];
    message[3] = check_bytes[1];

    return rpi_transmit(message, 4);
}

/**
 * @brief Send a HUMAN_MOVE instruction from the MSP432 to the Raspberry Pi
 * @param move A 5-character array containing the human's move in UCI notation
 *             (4 - 5 characters). If the UCI move is not 5 characters, the
 *             fifth character should be filled with '_'.
 *
 * @return true if the transmission was successful, false otherwise
 */
bool rpi_transmit_human_move(char move[5])
{
    char message[9];
    char check_bytes[2];

    message[0] = START_BYTE;
    message[1] = HUMAN_MOVE_INSTR_AND_LEN;
    message[2] = move[0];
    message[3] = move[1];
    message[4] = move[2];
    message[5] = move[3];
    message[6] = move[4];
    utils_fl16_data_to_cbytes((uint8_t *) message, 7, check_bytes);
    message[7] = check_bytes[0];
    message[8] = check_bytes[1];
    return rpi_transmit(message, 9);
}

/**
 * @brief Clears the Tx and Rx fifos for RPi communication
 */
void rpi_reset_uart(void)
{
    uart_reset(RPI_UART_CHANNEL);
}

/**
 * @brief Given one of four possible castle moves, returns the corresponding move
 *        the rook will make.
 *
 * @param king_move The castle move, which describes how the king moves
 * @return The corresponding rook move to go with whatever king move was passed in
 */
chess_move_t rpi_get_castle_rook_move(chess_move_t *king_move)
{
    chess_rank_t d_r = king_move->dest_rank;
    chess_file_t d_f = king_move->dest_file;

    chess_move_t rook_move;

    // White side castle
    if (d_r == FIRST)
    {
        // White queen-side castle
        // King's move: e1c1 | Rook's move: a1d1
        if (d_f == C)
        {
            rook_move.source_file = A;
            rook_move.source_rank = FIRST;
            rook_move.dest_file = D;
            rook_move.dest_rank = FIRST;
            rook_move.move_type = MOVE;
            return rook_move;
        }
        // White king-side castle
        // King's move: e1g1 | Rook's move: h1f1
        else if (d_f == G)
        {
            rook_move.source_file = H;
            rook_move.source_rank = FIRST;
            rook_move.dest_file = F;
            rook_move.dest_rank = FIRST;
            rook_move.move_type = MOVE;
            return rook_move;
        }
    }
    // Black side castle
    else if (d_r == EIGHTH)
    {
        // Black queen-side castle
        // King's move: e8c8 | Rook's move: a8d8
        if (d_f == C)
        {
            rook_move.source_file = A;
            rook_move.source_rank = EIGHTH;
            rook_move.dest_file = D;
            rook_move.dest_rank = EIGHTH;
            rook_move.move_type = MOVE;
            return rook_move;
        }
        // Black king-side castle
        // King's move: e8g8 | Rook's move: h8f8
        else if (d_f == G)
        {
            rook_move.source_file = H;
            rook_move.source_rank = EIGHTH;
            rook_move.dest_file = F;
            rook_move.dest_rank = EIGHTH;
            rook_move.move_type = MOVE;
            return rook_move;
        }
    }
    else
    {
        return *king_move;
    }

    return *king_move;
}

/* End raspberrypi.c */
