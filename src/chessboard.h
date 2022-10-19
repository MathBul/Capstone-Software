/**
 * @file chessboard.h
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Code for defining the 'chess_board_t' struct, which carries presence and piece data
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CHESSBOARD_H_
#define CHESSBOARD_H_

typedef struct {
    long board_presence;
    char board_pieces[8][8];
} chess_board_t;

void chess_board_init(chess_board_t *board);
uint8_t square_to_index(char rank, char file);
char* index_to_square(uint8_t index, char square[2]);
char* get_move(chess_board_t* previous, chess_board_t* current, char move[5]);
void chessboard_reset();

#endif /* CHESSBOARD_H_ */
