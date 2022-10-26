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

static chess_board_t previous_board;
static chess_board_t current_board;

/**
 * @brief Initialize chess_board_t presence and piece boards with default values
 * @param chess_board_t *board A pointer to the chess_board_t to be initialized
 */
void chessboard_init(chess_board_t *board)
{
    // Set default board presence
    board->board_presence = (uint64_t) 0xFFFF00000000FFFF;

    // Set white's non-pawn pieces
    board->board_pieces[0][0] = 'R';
    board->board_pieces[0][1] = 'N';
    board->board_pieces[0][2] = 'B';
    board->board_pieces[0][3] = 'Q';
    board->board_pieces[0][4] = 'K';
    board->board_pieces[0][5] = 'B';
    board->board_pieces[0][6] = 'N';
    board->board_pieces[0][7] = 'R';

    // Counter variable
    int i;

    /* Set white's pawns, the empty 4 ranks in the middle of
       the board, then black's pawns */
    for (i = 0; i < 8; i++)
    {
        board->board_pieces[1][i] = 'P';
        board->board_pieces[2][i] = '\0';
        board->board_pieces[3][i] = '\0';
        board->board_pieces[4][i] = '\0';
        board->board_pieces[5][i] = '\0';
        board->board_pieces[6][i] = 'p';
    }

    // Set black's non-pawn pieces
    board->board_pieces[7][0] = 'r';
    board->board_pieces[7][1] = 'n';
    board->board_pieces[7][2] = 'b';
    board->board_pieces[7][3] = 'q';
    board->board_pieces[7][4] = 'k';
    board->board_pieces[7][5] = 'b';
    board->board_pieces[7][6] = 'n';
    board->board_pieces[7][7] = 'r';
}

/**
 * @brief Convert a chess square to its index (0 - 63)
 * @param char file The char for the square's file
 * @param char rank The char for the square's rank
 * @return The integer representation of the square passed in
 */
uint8_t chessboard_square_to_index(char file, char rank)
{
    uint8_t index = 0;
    switch (rank)
    {
        case '1':
            index += 0;
            break;
        case '2':
            index += 8;
            break;
        case '3':
            index += 16;
            break;
        case '4':
            index += 24;
            break;
        case '5':
            index += 32;
            break;
        case '6':
            index += 40;
            break;
        case '7':
            index += 48;
            break;
        case '8':
            index += 56;
            break;
        default:
            break;
    }
    switch(file)
    {
        case 'a':
            index += 0;
            break;
        case 'b':
            index += 1;
            break;
        case 'c':
            index += 2;
            break;
        case 'd':
            index += 3;
            break;
        case 'e':
            index += 4;
            break;
        case 'f':
            index += 5;
            break;
        case 'g':
            index += 6;
            break;
        case 'h':
            index += 7;
            break;
        default:
            break;
    }
    return index;
}

/**
 * @brief Convert an index (0 - 63) to its chess square
 * @param uint8_t index The index to be converted
 * @param char square[2] A buffer for this method to write the square into
 * @return A char array containing the square
 */
char* chessboard_index_to_square(uint8_t index, char square[2])
{
    if (index > 63)
    {
        square[0] = '?';
        square[1] = '?';
        return square;
    }
    char file = (index % 8) + 'a';
    char rank = (index / 8) + '1';
    square[0] = file;
    square[1] = rank;
    return square;
}

/**
 * @brief Determine the move the human made (in UCI notation) by comparing
 *        the previous board to the current board.
 * @param chess_board_t* previous The previous state of the board
 * @param chess_board_t* current The new/current state of the board
 * @param move[5] A buffer for this method to write the move into
 */
char* chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5])
{
    // Get presence boards
    uint64_t previous_presence = previous->board_presence;
    uint64_t current_presence = current->board_presence;

    // Find the changes in presence
    uint64_t changes_in_presence = previous_presence ^ current_presence;
    uint8_t num_changes_in_presence = 0;

    int i = 0;
    // Indices of changes (initialized to impossible values for indices)
    uint8_t index1 = 0xFF;
    uint8_t index2 = 0xFF;

    // Where to temporarily store the initial and final squares
    char square_initial[2];
    char square_final[2];

    // Find set bits (1s) in changes_in_presence
    for (i = 0; i < 64; i++)
    {
        // If a set bit is found in some bit position i, store its index
        if ((changes_in_presence >> i) & 0x01)
        {
            num_changes_in_presence += 1;
            // On the first set bit seen, store in index1
            if (index1 == 0xFF)
            {
                index1 = i;
            }
            // On the second set bit seen, store in index2
            else
            {
                index2 = i;
                break;
            }
        }
    }

    /* If index1 was a 1 on the previous board, a piece was there, meaning it was
       the initial square */
    if ((previous_presence >> index1) & 0x01)
    {
        chessboard_index_to_square(index1, square_initial);
        chessboard_index_to_square(index2, square_final);
    }
    /* Otherwise, index1 was a 0, meaning no piece was there, meaning it was the
       final square */
    else
    {
        chessboard_index_to_square(index2, square_initial);
        chessboard_index_to_square(index1, square_final);
    }
    move[0] = square_initial[0];
    move[1] = square_initial[1];
    move[2] = square_final[0];
    move[3] = square_final[1];
    move[4] = '_';
    return move;
}

/**
 * @brief Reset both chess boards to their default states
 */
void chessboard_reset(void)
{
    chessboard_init(&previous_board);
    chessboard_init(&current_board);
}

/* End chessboard.c */
