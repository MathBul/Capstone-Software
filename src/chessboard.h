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

typedef struct {
    uint64_t board_presence;
    char board_pieces[8][8];
} chess_board_t;

void chessboard_init(chess_board_t *board);
uint8_t chessboard_square_to_index(char file, char rank);
char* chessboard_index_to_square(uint8_t index, char square[2]);
char* chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5]);
void chessboard_reset(void);

#endif /* CHESSBOARD_H_ */
