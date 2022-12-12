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
#define INITIAL_PRESENCE_WHITE              ((uint64_t) 0x000000000000FFFF)
#define INITIAL_PRESENCE_BLACK              ((uint64_t) 0xFFFF000000000000)
#define INITIAL_PRESENCE_BOARD              (INITIAL_PRESENCE_WHITE | INITIAL_PRESENCE_BLACK)

// Possible castling signatures
#define CASTLE_WHITE_K                      (0x00000000000000F0)    // (e1g1)
#define CASTLE_WHITE_Q                      (0x000000000000001D)    // (e1c1)
#define CASTLE_BLACK_K                      (0xF000000000000000)    // (e8g8)
#define CASTLE_BLACK_Q                      (0x1D00000000000000)    // (e8c8)

// Board state struct
typedef struct {
    uint64_t board_presence;
    uint64_t robot_presence;
    uint64_t human_presence;
    char board_pieces[8][8];
} chess_board_t;

// Board changes struct
typedef struct board_changes_t {
    uint8_t num_changes;
    uint8_t presence_change_index_1;
    uint8_t presence_change_index_2;
    uint8_t presence_change_index_3;
    uint8_t presence_change_index_4;
} board_changes_t;

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
void chessboard_init(void);
void chessboard_reset_all(void);
chess_piece_t chessboard_get_piece_at_position(chess_file_t file, chess_rank_t rank);
bool chessboard_update_intermediate_board_from_presence(uint64_t board_reading, char move[5]);
bool chessboard_update_current_board_from_presence(uint64_t board_reading, char move[5], bool capture);
void chessboard_update_previous_board_from_current_board(void);
void chessboard_update_current_board_from_previous_board(void);
void chessboard_update_previous_board_from_move(char move[5]);
void chessboard_update_current_board_from_move(char move[5]);

#endif /* CHESSBOARD_H_ */
