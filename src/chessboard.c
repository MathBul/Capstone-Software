/**
 * @file chessboard.c
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Defines useful methods related to processing data with chess_board_t structs
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdint.h>
#include "chessboard.h"

void chess_board_init(chess_board_t *board)
{
    board->board_presence = (long) 0xFFFF00000000FFFF;

    board->board_pieces[0][0] = 'R';
    board->board_pieces[0][1] = 'N';
    board->board_pieces[0][2] = 'B';
    board->board_pieces[0][3] = 'Q';
    board->board_pieces[0][4] = 'K';
    board->board_pieces[0][5] = 'B';
    board->board_pieces[0][6] = 'N';
    board->board_pieces[0][7] = 'R';

    int i;

    for (i = 0; i < 8; i++)
    {
        board->board_pieces[1][i] = 'P';
        board->board_pieces[2][i] = '\0';
        board->board_pieces[3][i] = '\0';
        board->board_pieces[4][i] = '\0';
        board->board_pieces[5][i] = '\0';
        board->board_pieces[6][i] = 'p';
    }

    board->board_pieces[7][0] = 'r';
    board->board_pieces[7][1] = 'n';
    board->board_pieces[7][2] = 'b';
    board->board_pieces[7][3] = 'q';
    board->board_pieces[7][4] = 'k';
    board->board_pieces[7][5] = 'b';
    board->board_pieces[7][6] = 'n';
    board->board_pieces[7][7] = 'r';
}

uint8_t square_to_index(char rank, char file)
{
    uint8_t index = 0;
    switch (rank)
    {
        case 'a':
            index += 0;
            break;
        case 'b':
            index += 8;
            break;
        case 'c':
            index += 16;
            break;
        case 'd':
            index += 24;
            break;
        case 'e':
            index += 32;
            break;
        case 'f':
            index += 40;
            break;
        case 'g':
            index += 48;
            break;
        case 'h':
            index += 56;
            break;
        default:
            break;
    }
    switch(file)
    {
        case '1':
            index += 0;
            break;
        case '2':
            index += 1;
            break;
        case '3':
            index += 2;
            break;
        case '4':
            index += 3;
            break;
        case '5':
            index += 4;
            break;
        case '6':
            index += 5;
            break;
        case '7':
            index += 6;
            break;
        case '8':
            index += 7;
            break;
        default:
            break;
    }
    return index;
}

char* index_to_square(uint8_t index, char square[2])
{
    if (index > 63)
    {
        square[0] = '?';
        square[1] = '?';
        return square;
    }
    char file = (index / 8) + 'a';
    char rank = (index % 8) + '1';
    square[0] = file;
    square[1] = rank;
    return square;
}

char* get_move(chess_board_t* previous, chess_board_t* current, char move[5])
{
    long previous_presence = previous->board_presence;
    long current_presence = current->board_presence;

    long changes_in_presence = previous_presence ^ current_presence;
    uint8_t num_changes_in_presence = 0;

    int i = 0;
    uint8_t index1 = 0xFF;
    uint8_t index2 = 0xFF;
    char square_initial[2];
    char square_final[2];

    for (i = 0; i < 64; i++)
    {
        if ((changes_in_presence >> i) & 0x01)
        {
            num_changes_in_presence += 1;
            if (index1 == 0xFF)
            {
                index1 = i;
            }
            else
            {
                index2 = i;
                break;
            }
        }
        i++;
    }

    // If the first changed index was a 1 on the previous board
    if ((previous_presence >> index1) & 0x01)
    {
        index_to_square(index1, square_initial);
        index_to_square(index2, square_final);
    }
    // Otherwise, the squares are reversed
    else
    {
        index_to_square(index2, square_initial);
        index_to_square(index1, square_final);
    }
    move[0] = square_initial[0];
    move[1] = square_initial[1];
    move[2] = square_final[0];
    move[3] = square_final[1];
    move[4] = '_';
    return move;
}
