/**
 * @file chessboard.c
 * @author Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for processing chess-related data
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 */

#include "chessboard.h"

// Private functions
static void chessboard_reset_board(chess_board_t *board);
static uint8_t chessboard_tile_to_index(char file, char rank);
static char* chessboard_index_to_tile(uint8_t index, char square[2]);
static uint8_t chessboard_index_to_file_index(uint8_t index);
static uint8_t chessboard_index_to_rank_index(uint8_t index);
static bool chessboard_is_promotion(char initial_rank, char final_rank, char moving_piece);
static void chessboard_update_presence_move(chess_board_t* p_board, char move[5]);
static void chessboard_update_presence_board(chess_board_t* p_board, char move[5]);
static void chessboard_update_pieces_move(chess_board_t *board, char move[5]);
static void chessboard_update_pieces_board(chess_board_t *board, char move[5]);
static bool chessboard_update_pieces_castling(chess_board_t *board, uint64_t castling_signature);
static void chessboard_castle_get_rook_move(char move[5], char rook_move[5]);

// Previous, intermediate (case of captures), and current boards
chess_board_t chessboards[NUMBER_OF_CHESSBOARDS];
static chess_board_t* p_prev_board = &chessboards[0];
static chess_board_t* p_capt_board = &chessboards[1];
static chess_board_t* p_curr_board = &chessboards[2];

/**
 * @brief Inititialzes all chessboards
 */
void chessboard_init()
{
    chessboard_reset_all();
}

/**
 * @brief Resets a chessboard to its default state
 *
 * @param chess_board_t Pointer to the chessboard being reset
 */
static void chessboard_reset_board(chess_board_t *board)
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

    // Set white's pawns, the empty 4 ranks in the middle of the board, then black's pawns
    uint8_t i = 0;
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
 * @brief Reset all chess boards to their default states
 */
void chessboard_reset_all()
{
    uint8_t i = 0;
    for (i = 0; i < NUMBER_OF_CHESSBOARDS; i++)
    {
        chessboard_reset_board(&chessboards[i]);
    }
}

/**
 * @brief Convert a chess tile to its index (0 - 63)
 *
 * @param file The char for the tile's file
 * @param rank The char for the tile's rank
 * @return The integer representation of the tile passed in
 */
static uint8_t chessboard_tile_to_index(char file, char rank)
{
    return utils_tile_to_index(utils_byte_to_file(file), utils_byte_to_rank(rank));
}

/**
 * @brief Convert an index (0 - 63) to its rank and file (stored as a char array)
 *
 * @param index The index to be converted
 * @param tile Buffer for this method to write the tile into (file, rank)
 * @return A char array containing the tile
 */
static char* chessboard_index_to_tile(uint8_t index, char tile[2])
{
    // If index exceeds bounds, return error tile
    if (index > 63)
    {
        tile[0] = '?';
        tile[1] = '?';
        return tile;
    }

    // Compute rank and file
    char file = (index % 8) + 'a';
    char rank = (index / 8) + '1';
    tile[0] = file;
    tile[1] = rank;

    return tile;
}

/**
 * @brief Convert an index (0 - 63) to its file index (0 - 8)
 *
 * @param index The index to be converted
 * @return One of {0,...,7} corresponding to the file of the index passed in
 */
static uint8_t chessboard_index_to_file_index(uint8_t index)
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
 * @param index The index to be converted
 * @return One of {0,...,7} corresponding to the rank of the index passed in
 */
static uint8_t chessboard_index_to_rank_index(uint8_t index)
{
    if (index > 63)
    {
        // Something has gone wrong
        return index;
    }
    return (index / 8);
}

/**
 * @brief Checks if the given move is a promotion
 *
 * @param initial_rank The initial rank, represented as a char
 * @param final_rank The final rank, represented as a char
 * @param moving_piece The piece that is moving from initial_rank to final_rank *
 * @return Whether the move is a promotion
 */
static bool chessboard_is_promotion(char initial_rank, char final_rank, char moving_piece)
{
    // Only pawns can promote
    if ((moving_piece != 'P') && (moving_piece != 'p'))
    {
        return false;
    }

    // Pawns can be promoted when moving from the second rank to first rank
    if ((initial_rank == '2') && (final_rank == '1'))
    {
        return true;
    }

    // Pawns can be promoted when moving from the seventh rank to the eighth rank
    if ((initial_rank == '7') && (final_rank == '8'))
    {
        return true;
    }

    // Not a promotion
    return false;
}

/**
 * @brief Determine the human's move (in UCI notation) by comparing the previous and current boards
 *
 * @param chess_board_t* previous The previous state of the board
 * @param chess_board_t* current The new/current state of the board
 * @param move A buffer to write the move into *
 * @return Whether the move was (likely) legal (will know for sure once checked by the chess engine)
 */
bool chessboard_get_move(chess_board_t* previous, chess_board_t* current, char move[5])
{
    // Find the changes in presence
    uint64_t previous_presence   = previous->board_presence;
    uint64_t current_presence    = current->board_presence;
    uint64_t changes_in_presence = (previous_presence ^ current_presence);

    // Get the indices of the changes
    board_changes_t board_changes;
    utils_get_board_changes(changes_in_presence, &board_changes);

    // Translate the changes to a move
    bool legality = false;
    if (board_changes.num_changes == 2)             // Non-special move
    {
        char tile_initial[2];
        char tile_final[2];
        uint8_t index1 = board_changes.index1;
        uint8_t index2 = board_changes.index2;

        // Determine which index was the initial tile
        uint8_t initial_index;
        if ((previous_presence >> index1) & 0x01)   // If index1 was 1 on previous board, a piece moved from there
        {
            chessboard_index_to_tile(index1, tile_initial);
            chessboard_index_to_tile(index2, tile_final);
            initial_index = index1;
        }
        else                                        // Otherwise, the piece moved to index 1
        {
            chessboard_index_to_tile(index1, tile_final);
            chessboard_index_to_tile(index2, tile_initial);
            initial_index = index2;
        }

        // Fill the move buffer
        move[0] = tile_initial[0];
        move[1] = tile_initial[1];
        move[2] = tile_final[0];
        move[3] = tile_final[1];

        // Determine if this move was a promotion
        uint8_t initial_file_index = chessboard_index_to_file_index(initial_index);
        uint8_t initial_rank_index = chessboard_index_to_rank_index(initial_index);

        if (chessboard_is_promotion(tile_initial[1], tile_final[1], current->board_pieces[initial_rank_index][initial_file_index]))
        {
            move[4] = 'Q';
        }
        else
        {
            move[4] = '_';
        }

        // Update the pieces
        chessboard_update_pieces_move(current, move);
        legality = true;
    }
    else if (board_changes.num_changes == 4)        // Castling move, check the possible signatures
    {
        switch (changes_in_presence)
        {
            case CASTLE_WHITE_K:
                move[0] = 'e';
                move[1] = '1';
                move[2] = 'g';
                move[3] = '1';
                move[4] = '_';
                chessboard_update_pieces_castling(current, CASTLE_WHITE_K);
                legality = true;
            break;

            case CASTLE_WHITE_Q:
                move[0] = 'e';
                move[1] = '1';
                move[2] = 'c';
                move[3] = '1';
                move[4] = '_';
                chessboard_update_pieces_castling(current, CASTLE_WHITE_Q);
                legality = true;
            break;

            case CASTLE_BLACK_K:
                move[0] = 'e';
                move[1] = '8';
                move[2] = 'g';
                move[3] = '8';
                move[4] = '_';
                chessboard_update_pieces_castling(current, CASTLE_BLACK_K);
                legality = true;
            break;

            case CASTLE_BLACK_Q:
                move[0] = 'e';
                move[1] = '8';
                move[2] = 'c';
                move[3] = '8';
                move[4] = '_';
                chessboard_update_pieces_castling(current, CASTLE_BLACK_Q);
                legality = true;
            break;

            default:
                // Must be an illegal move
                legality = false;
            break;
        }
    }
    else
    {
        // Must be an illegal move
        legality = false;
    }

    return legality;
}

/**
 * @brief Update a board's presence based on a raw move
 *
 * @param board The board to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_presence_move(chess_board_t* p_board, char move[5])
{
    // Get the initial and final indices for this move
    uint8_t index_initial = chessboard_tile_to_index(move[0], move[1]);
    uint8_t index_final = chessboard_tile_to_index(move[2], move[3]);

    // The initial square becomes 0 since the piece has moved away from there
    p_board->board_presence &= (~(0x0000000000000001) << index_initial);
    
    // The final square becomes 1 since the piece has moved there
    p_board->board_presence |= ((0x0000000000000001) << index_final);
}

/**
 * @brief Update an entire board's presence from a move (e.g., accounts for possible castling)
 *
 * @param board The board to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_presence_board(chess_board_t* p_board, char move[5])
{
    // Check for castling move to update the board with the rook's move as well
    if (move[4] == 'c')
    {
        // Update for the corresponding rook move
        char rook_move[5];
        chessboard_castle_get_rook_move(move, rook_move);
        chessboard_update_presence_move(p_board, rook_move);
    }

    // Update for the specified move
    chessboard_update_presence_move(p_board, move);
}

/**
 * @brief Updates a board's pieces based on a raw move
 *
 * @param board The chessboard to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_pieces_move(chess_board_t *board, char move[5])
{
    // Separate the initial and final squares into rank and file pairs
    char tile_initial[2];
    char tile_final[2];

    // Assign values to the tiles from the move passed in
    tile_initial[0] = move[0];
    tile_initial[1] = move[1];
    tile_final[0]   = move[2];
    tile_final[1]   = move[3];

    // Get indices for the tiles
    uint8_t tile_initial_index = chessboard_tile_to_index(tile_initial[0], tile_initial[1]);
    uint8_t tile_final_index = chessboard_tile_to_index(tile_final[0], tile_final[1]);

    // Convert these indices to indices in the board_pieces 2D array
    uint8_t tile_initial_file_index = chessboard_index_to_file_index(tile_initial_index);
    uint8_t tile_initial_rank_index = chessboard_index_to_rank_index(tile_initial_index);
    uint8_t tile_final_file_index   = chessboard_index_to_file_index(tile_final_index);
    uint8_t tile_final_rank_index   = chessboard_index_to_rank_index(tile_final_index);

    // Update the board
    char moving_piece = board->board_pieces[tile_initial_rank_index][tile_initial_file_index];

    // Update the final position, and account for promotion
    board->board_pieces[tile_final_rank_index][tile_final_file_index] = moving_piece;

    if ((move[4] == 'Q') || (move[4] == 'q'))
    {
        // 'p' + 1 = 'q' and 'P' + 1 = 'Q', so this works for both colors
        board->board_pieces[tile_final_rank_index][tile_final_file_index] += 1;
    }

    // The initial square is now empty
    board->board_pieces[tile_initial_rank_index][tile_initial_file_index] = '\0';
}



/**
 * @brief Update an entire board's pieces from a move
 *
 * @param board The chessboard to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_board_pieces(chess_board_t *board, char move[5])
{
    // Check for castling move to update the board with the rook's move as well
    if (move[4] == 'c')
    {
        char rook_move[5];
        chessboard_castle_get_rook_move(move, rook_move);
        chessboard_update_pieces_move(board, rook_move);
    }

    // Update for the specified move
    chessboard_update_pieces_move(board, move);
}

/**
 * @brief Update a board's pieces according to a castling operation
 *
 * @param board The board to apply the castling move to
 * @param castling_signature One of four possible castling signatures
 * @return Whether the castling signature was valid
 */
static bool chessboard_update_pieces_castling(chess_board_t *board, uint64_t castling_signature)
{
    bool legality = true;

    // Update the pieces
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
            legality = false;
        break;
    }
    return legality;
}

/**
 * @brief Updates the chessboards following the robot's move
 * 
 * @param move The move made by the robot
 */
void chessboard_update_robot_move(char move[5])
{
    chessboard_update_presence_board(p_prev_board, move);
    chessboard_update_board_pieces(p_prev_board, move);
}

/**
 * @brief Takes a king's castling move and writes the corresponding rook's move into a buffer.
 *
 * @param move The king's castling move
 * @param rook_move The buffer to write the corresponding rook's move into
 */
void chessboard_castle_get_rook_move(char move[5], char rook_move[5])
{
    // White king-side castle. King's move: e1g1 <=> Rook's move: h1f1
    if ((move[0] == 'e') && (move[1] == '1') && (move[2] == 'g') && (move[3] == '1'))
    {
        rook_move[0] = 'h';
        rook_move[1] = '1';
        rook_move[2] = 'f';
        rook_move[3] = '1';
    }
    // White queen-side castle. King's move: e1c1 <=> Rook's move: a1d1
    else if ((move[0] == 'e') && (move[1] == '1') && (move[2] == 'c') && (move[3] == '1'))
    {
        rook_move[0] = 'a';
        rook_move[1] = '1';
        rook_move[2] = 'd';
        rook_move[3] = '1';
    }
    // Black king-side castle. King's move: e8g8 <=> Rook's move: h8f8
    else if ((move[0] == 'e') && (move[1] == '8') && (move[2] == 'g') && (move[3] == '8'))
    {
        rook_move[0] = 'h';
        rook_move[1] = '8';
        rook_move[2] = 'f';
        rook_move[3] = '8';
    }
    // Black queen-side castle. King's move: e8c8 <=> Rook's move: a8d8
    else if ((move[0] == 'e') && (move[1] == '8') && (move[2] == 'c') && (move[3] == '8'))
    {
        rook_move[0] = 'a';
        rook_move[1] = '8';
        rook_move[2] = 'd';
        rook_move[3] = '8';
    }
    // Should never get here; for debugging purposes
    else
    {
        rook_move[0] = '?';
        rook_move[1] = '?';
        rook_move[2] = '?';
        rook_move[3] = '?';
    }

    // Pad with '_'
    rook_move[4] = '_';
}

/* End chessboard.c */
