/**
 * @file chessboard.h
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Code for defining the 'chess_board_t' struct, which carries presence
 *        and piece data; additionally, defines methods helpful to using this
 *        type of struct
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CHESSBOARD_H_
#define CHESSBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

/* Changes in board presence that correspond to particular castling moves,
 * AKA castling signatures */
// 00000000 00000000 00000000 00000000 00000000 00000000 00000000 11110000 (e1g1)
#define CASTLE_WHITE_K (0xF0)

// 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00011101 (e1c1)
#define CASTLE_WHITE_Q (0x1D)

// 11110000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 (e8g8)
#define CASTLE_BLACK_K (0xF000000000000000)

// 00011101 00000000 00000000 00000000 00000000 00000000 00000000 00000000 (e8c8)
#define CASTLE_BLACK_Q (0x1D00000000000000)

typedef struct {
    uint64_t board_presence;
    char board_pieces[8][8];
} chess_board_t;

// Rank and file defines for indexing into the board_pieces 2D array
#define FIRST_RANK    0
#define SECOND_RANK   1
#define THIRD_RANK    2
#define FOURTH_RANK   3
#define FIFTH_RANK    4
#define SIXTH_RANK    5
#define SEVENTH_RANK  6
#define EIGHTH_RANK   7
#define A_FILE        0
#define B_FILE        1
#define C_FILE        2
#define D_FILE        3
#define E_FILE        4
#define F_FILE        5
#define G_FILE        6
#define H_FILE        7

void chessboard_init(chess_board_t *board);
uint8_t chessboard_square_to_index(char file, char rank);
char* chessboard_index_to_square(uint8_t index, char square[2]);
uint8_t chessboard_ind_to_file_ind(uint8_t index);
uint8_t chessboard_ind_to_rank_ind(uint8_t index);
bool chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5]);
void chessboard_update_pieces(chess_board_t *board, char move[5]);
bool chessboard_update_pcs_castling(chess_board_t *board, uint64_t castling_signature);
bool chessboard_is_promotion(char initial_rank, char final_rank, char moving_piece);
void chessboard_reset(chess_board_t *board);

#endif /* CHESSBOARD_H_ */
