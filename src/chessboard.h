/**
 * @file chessboard.h
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for processing chess-related data
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 */

#ifndef CHESSBOARD_H_
#define CHESSBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

// Note on chessboards:
//  - Due to a lack of physical resources, we do not allow underpromotion, only queening

// General chessboard macros
#define NUMBER_OF_CHESSBOARDS               (3)

// Possible castling signatures
#define CASTLE_WHITE_K                      (0x00000000000000F0)    // (e1g1)
#define CASTLE_WHITE_Q                      (0x000000000000001D)    // (e1c1)
#define CASTLE_BLACK_K                      (0xF000000000000000)    // (e8g8)
#define CASTLE_BLACK_Q                      (0x1D00000000000000)    // (e8c8)

// Board state struct
typedef struct {
    uint64_t board_presence;
    char board_pieces[8][8];
} chess_board_t;

// Rank and file defines for indexing into the board_pieces 2D array
#define FIRST_RANK                          (0)
#define SECOND_RANK                         (1)
#define THIRD_RANK                          (2)
#define FOURTH_RANK                         (3)
#define FIFTH_RANK                          (4)
#define SIXTH_RANK                          (5)
#define SEVENTH_RANK                        (6)
#define EIGHTH_RANK                         (7)
#define A_FILE                              (0)
#define B_FILE                              (1)
#define C_FILE                              (2)
#define D_FILE                              (3)
#define E_FILE                              (4)
#define F_FILE                              (5)
#define G_FILE                              (6)
#define H_FILE                              (7)

// Public functions
void chessboard_init();
void chessboard_reset_all();
bool chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5]);
void chessboard_update_robot_move(char move[5]);

#endif /* CHESSBOARD_H_ */
