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

#include "chessboard.h"

/**
 * @brief Initialize chess_board_t presence and piece boards with default values
 *
 * @param chess_board_t *board A pointer to the chess_board_t to be initialized
 */
void chessboard_init(chess_board_t *board)
{
    // Set default board presence
    board->board_presence = (uint64_t) 0xFFFF00000000FFFF;

    // Set white's non-pawn pieces
    board->board_pieces[FIRST_RANK][A_FILE] = 'R';
    board->board_pieces[FIRST_RANK][B_FILE] = 'N';
    board->board_pieces[FIRST_RANK][C_FILE] = 'B';
    board->board_pieces[FIRST_RANK][D_FILE] = 'Q';
    board->board_pieces[FIRST_RANK][E_FILE] = 'K';
    board->board_pieces[FIRST_RANK][F_FILE] = 'B';
    board->board_pieces[FIRST_RANK][G_FILE] = 'N';
    board->board_pieces[FIRST_RANK][H_FILE] = 'R';

    // Counter variable
    int i;

    /* Set white's pawns, the empty 4 ranks in the middle of
       the board, then black's pawns */
    for (i = 0; i < 8; i++)
    {
        board->board_pieces[SECOND_RANK][i]  = 'P';
        board->board_pieces[THIRD_RANK][i]   = '\0';
        board->board_pieces[FOURTH_RANK][i]  = '\0';
        board->board_pieces[FIFTH_RANK][i]   = '\0';
        board->board_pieces[SIXTH_RANK][i]   = '\0';
        board->board_pieces[SEVENTH_RANK][i] = 'p';
    }

    // Set black's non-pawn pieces
    board->board_pieces[EIGHTH_RANK][A_FILE] = 'r';
    board->board_pieces[EIGHTH_RANK][B_FILE] = 'n';
    board->board_pieces[EIGHTH_RANK][C_FILE] = 'b';
    board->board_pieces[EIGHTH_RANK][D_FILE] = 'q';
    board->board_pieces[EIGHTH_RANK][E_FILE] = 'k';
    board->board_pieces[EIGHTH_RANK][F_FILE] = 'b';
    board->board_pieces[EIGHTH_RANK][G_FILE] = 'n';
    board->board_pieces[EIGHTH_RANK][H_FILE] = 'r';
}

/**
 * @brief Convert a chess square to its index (0 - 63)
 *
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
 *
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
 * @brief Convert an index (0 - 63) to its file index (0 - 8)
 *
 * @param uint8_t index The index to be converted
 *
 * @return An unsigned integer corresponding to the file of the index passed in
 */
uint8_t chessboard_ind_to_file_ind(uint8_t index)
{
    if (index > 63)
    {
        // Something has gone wrong
        return index;
    }
    return (index % 8);
}

/**
 * @brief Convert an index (0 - 63) to its rank index (0 - 8)
 *
 * @param uint8_t index The index to be converted
 *
 * @return An unsigned integer corresponding to the rank of the index passed in
 */
uint8_t chessboard_ind_to_rank_ind(uint8_t index)
{
    if (index > 63)
    {
        // Something has gone wrong
        return index;
    }
    return (index / 8);
}

/**
 * @brief Determine the move the human made (in UCI notation) by comparing
 *        the previous board to the current board.
 *
 * @param chess_board_t* previous The previous state of the board
 * @param chess_board_t* current The new/current state of the board
 * @param move[5] A buffer for this method to write the move into
 *
 * @return false if move is definitely illegal, true otherwise
 */
bool chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5])
{
    // Get presence boards
    uint64_t previous_presence = previous->board_presence;
    uint64_t current_presence = current->board_presence;

    // Find the changes in presence
    uint64_t changes_in_presence = previous_presence ^ current_presence;
    // Declare board_changes struct and set default values
    board_changes_t board_changes;
    board_changes.num_changes = 0;
    board_changes.index1 = 0xFF;
    board_changes.index2 = 0xFF;
    board_changes.index3 = 0xFF;
    board_changes.index4 = 0xFF;

    // Store the indices of changes in presence in board_changes struct
    utils_get_board_changes(changes_in_presence, &board_changes);
    uint8_t num_changes = board_changes.num_changes;

    // Likely a non-special move
    if (num_changes == 2)
    {
        // Where to temporarily store the initial and final squares
        char square_initial[2];
        char square_final[2];

        // Indices of changed sqaures
        uint8_t index1 = board_changes.index1;
        uint8_t index2 = board_changes.index2;

        /* If index1 was a 1 on the previous board, a piece was there, meaning it was
           the initial square */
        uint8_t sq_initial_index;
        if((previous_presence >> index1) & 0x01)
        {
            chessboard_index_to_square(index1, square_initial);
            chessboard_index_to_square(index2, square_final);
            sq_initial_index = index1;
        }
        /* Otherwise, index1 was a 0, meaning no piece was there, meaning it was the
           final square */
        else
        {
            chessboard_index_to_square(index1, square_final);
            chessboard_index_to_square(index2, square_initial);
            sq_initial_index = index2;
        }
        move[0] = square_initial[0];
        move[1] = square_initial[1];
        move[2] = square_final[0];
        move[3] = square_final[1];

        // Determine if this move is a promotion
        uint8_t sq_initial_file_ind = chessboard_ind_to_file_ind(sq_initial_index);
        uint8_t sq_initial_rank_ind = chessboard_ind_to_rank_ind(sq_initial_index);
        if (chessboard_is_promotion(square_initial[1], square_final[1],
                                    current->board_pieces[sq_initial_rank_ind][sq_initial_file_ind]))
        {
            move[4] = 'Q';
        }
        else
        {
            move[4] = '_';
        }
        // Update the pieces
        chessboard_update_pieces(current, move);
        return true;
    }
    // Likely a castling move, check for different changes in presence "signatures"
    else if (num_changes == 4)
    {
        if (changes_in_presence == CASTLE_WHITE_K)
        {
            move[0] = 'e';
            move[1] = '1';
            move[2] = 'g';
            move[3] = '1';
            move[4] = '_';
            chessboard_update_pcs_castling(current, CASTLE_WHITE_K);
            return true;
        }
        else if (changes_in_presence == CASTLE_WHITE_Q)
        {
            move[0] = 'e';
            move[1] = '1';
            move[2] = 'c';
            move[3] = '1';
            move[4] = '_';
            chessboard_update_pcs_castling(current, CASTLE_WHITE_Q);
            return true;
        }
        else if (changes_in_presence == CASTLE_BLACK_K)
        {
            move[0] = 'e';
            move[1] = '8';
            move[2] = 'g';
            move[3] = '8';
            move[4] = '_';
            chessboard_update_pcs_castling(current, CASTLE_BLACK_K);
            return true;
        }
        else if (changes_in_presence == CASTLE_BLACK_Q)
        {
            move[0] = 'e';
            move[1] = '8';
            move[2] = 'c';
            move[3] = '8';
            move[4] = '_';
            chessboard_update_pcs_castling(current, CASTLE_BLACK_Q);
            return true;
        }
        else
        {
            // Must be an illegal move
            return false;
        }
    }
    else
    {
        // Must be an illegal move
        return false;
    }
}

/**
 * @brief Updates a chess_board_t's board_pieces field by applying a
 *        given move to it.
 *
 * @param chess_board_t* board The board to apply the move to
 * @param move[5] The move in UCI notation (4-5 characters)
 */
void chessboard_update_pieces(chess_board_t *board, char move[5])
{
    // Separate the initial and final squares into rank and file pairs
    char square_initial[2];
    char square_final[2];

    // Assign the squares values from the move passed in
    square_initial[0] = move[0];
    square_initial[1] = move[1];
    square_final[0] = move[2];
    square_final[1] = move[3];

    // Get indices for the squares [0, 63]
    uint8_t square_initial_index = chessboard_square_to_index(square_initial[0], square_initial[1]);
    uint8_t square_final_index = chessboard_square_to_index(square_final[0], square_final[1]);

    // Convert these indices to indices in the board_pieces 2D array
    uint8_t sq_initial_file_index = chessboard_ind_to_file_ind(square_initial_index);
    uint8_t sq_initial_rank_index = chessboard_ind_to_rank_ind(square_initial_index);
    uint8_t sq_final_file_index = chessboard_ind_to_file_ind(square_final_index);
    uint8_t sq_final_rank_index = chessboard_ind_to_rank_ind(square_final_index);

    // Update the board
    char moving_piece = board->board_pieces[sq_initial_file_index][sq_initial_rank_index];
    // Update the final position, and account for promotion
    if ((move[4] == 'Q') || (move[4] == 'q'))
    {
        // 'p' + 1 = 'q' and 'P' + 1 = 'Q', so this works for both colors
        board->board_pieces[sq_final_rank_index][sq_final_file_index] = moving_piece + 1;
    }
    else
    {
        // Otherwise, the moving piece should stay the same
        board->board_pieces[sq_final_rank_index][sq_final_file_index] = moving_piece;
    }
    // The initial square is now empty
    board->board_pieces[sq_initial_rank_index][sq_initial_file_index] = '\0';
}

/**
 * @brief Updates a chess_board_t's board_pieces field by applying a given
 *        castling move to it. Uses castling signatures defined in chessboard.h
 *        to determine which pieces to move.
 *
 * @param chess_board_t* board The board to apply the castling move to
 * @param castling_signature One of the four castling signatures defined in chessboard.h
 *
 * @return true if a valid castling signature was given, false otherwise
 */
bool chessboard_update_pcs_castling(chess_board_t *board, uint64_t castling_signature)
{
    switch(castling_signature)
    {
        case CASTLE_WHITE_K:
            board->board_pieces[FIRST_RANK][E_FILE] = '\0';
            board->board_pieces[FIRST_RANK][F_FILE] = 'R';
            board->board_pieces[FIRST_RANK][G_FILE] = 'K';
            board->board_pieces[FIRST_RANK][H_FILE] = '\0';
            break;
        case CASTLE_WHITE_Q:
            board->board_pieces[FIRST_RANK][A_FILE] = '\0';
            board->board_pieces[FIRST_RANK][C_FILE] = 'K';
            board->board_pieces[FIRST_RANK][D_FILE] = 'R';
            board->board_pieces[FIRST_RANK][E_FILE] = '\0';
            break;
        case CASTLE_BLACK_K:
            board->board_pieces[EIGHTH_RANK][E_FILE] = '\0';
            board->board_pieces[EIGHTH_RANK][F_FILE] = 'r';
            board->board_pieces[EIGHTH_RANK][G_FILE] = 'k';
            board->board_pieces[EIGHTH_RANK][H_FILE] = '\0';
            break;
        case CASTLE_BLACK_Q:
            board->board_pieces[EIGHTH_RANK][A_FILE] = '\0';
            board->board_pieces[EIGHTH_RANK][C_FILE] = 'k';
            board->board_pieces[EIGHTH_RANK][D_FILE] = 'r';
            board->board_pieces[EIGHTH_RANK][E_FILE] = '\0';
            break;
        default:
            return false;
    }
    return true;
}

/**
 * @brief Tells if a particular piece moving from an initial rank to a final
 *        rank would be considered a promotion.
 *
 * @param initial_rank The initial rank, represented as a char
 * @param final_rank The final rank, represented as a char
 * @param moving_piece The piece that is moving from initial_rank to final_rank
 *
 * @return true if the move is a promotion, false if not
 */
bool chessboard_is_promotion(char initial_rank, char final_rank, char moving_piece)
{
    // Must be a pawn to promote; not a promotion
    if ((moving_piece != 'P') && (moving_piece != 'p'))
    {
        return false;
    }

    // Pawns moving from the second rank to first rank
    if ((initial_rank == '2') && (final_rank == '1'))
    {
        return true;
    }

    // Pawns moving from the seventh rank to the eighth rank
    if ((initial_rank == '7') && (final_rank == '8'))
    {
        return true;
    }

    // Not a promotion
    return false;
}


/**
 * @brief Reset both chess boards to their default states
 */
void chessboard_reset(chess_board_t *board)
{
    chessboard_init(board);
}

/* End chessboard.c */
