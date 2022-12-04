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
static uint8_t chessboard_tile_to_presence_index(char file, char rank);
static char* chessboard_presence_index_to_tile_buffer(uint8_t index, char square[2]);
static uint8_t chessboard_presence_index_to_file_index(uint8_t index);
static uint8_t chessboard_presence_index_to_rank_index(uint8_t index);
static bool chessboard_is_promotion(char initial_rank, char final_rank, char moving_piece);
static void chessboard_castle_get_rook_move(char move[5], char rook_move[5]);
static board_changes_t chessboard_get_board_changes_from_presence(uint64_t initial_presence, uint64_t final_presence);
static bool chessboard_get_move_from_presence(uint64_t initial_presence, uint64_t final_presence, char move[5]);
static uint64_t chessboard_get_presence_from_move(uint64_t initial_presence, char move[5]);
static void chessboard_update_pieces_from_move_activity(chess_board_t *p_board, char move[5]);
static void chessboard_update_pieces_from_move(chess_board_t *p_board, char move[5], bool human_move);
static bool chessboard_update_from_presence(chess_board_t* p_board, uint64_t new_presence, char move[5]);
static void chessboard_update_from_move(chess_board_t* p_board, char move[5]);
static void chessboard_copy_board(chess_board_t* p_source_board, chess_board_t* p_dest_board);

// Previous, intermediate (case of captures), and current boards
chess_board_t chessboards[NUMBER_OF_CHESSBOARDS];
static chess_board_t* p_prev_board  = &chessboards[0];
static chess_board_t* p_inter_board = &chessboards[1];
static chess_board_t* p_curr_board  = &chessboards[2];

/**
 * @brief Inititialzes all chessboards
 */
void chessboard_init(void)
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
    int i = 0;
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
void chessboard_reset_all(void)
{
    int i = 0;
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
static uint8_t chessboard_tile_to_presence_index(char file, char rank)
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
static char* chessboard_presence_index_to_tile_buffer(uint8_t index, char tile[2])
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
static uint8_t chessboard_presence_index_to_file_index(uint8_t index)
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
static uint8_t chessboard_presence_index_to_rank_index(uint8_t index)
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
 * @brief Takes a king's castling move and writes the corresponding rook's move into a buffer.
 *
 * @param move The king's castling move
 * @param rook_move The buffer to write the corresponding rook's move into
 */
static void chessboard_castle_get_rook_move(char move[5], char rook_move[5])
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

/**
 * @brief Get the difference in board state between two bit boards
 * 
 * @param initial_presence The initial bit board
 * @param final_presence The final bit board
 * @return The number of changes and the indices that changed
 */
static board_changes_t chessboard_get_board_changes_from_presence(uint64_t initial_presence, uint64_t final_presence)
{
    // Get the raw changes
    uint64_t presence_changes = (initial_presence ^ final_presence);

    // Initialize the struct
    board_changes_t board_changes;
    board_changes.num_changes             = 0;
    board_changes.presence_change_index_1 = 0xFF;
    board_changes.presence_change_index_2 = 0xFF;
    board_changes.presence_change_index_3 = 0xFF;
    board_changes.presence_change_index_4 = 0xFF;

    // Find which bits are set in the presence change
    uint8_t i = 0;
    for (i = 0; i < 64; i++)
    {
        // If a set bit is found in some bit position i, store its index
        if ((presence_changes >> i) & 0x01)
        {
            // Determine which change this is (we allow up to four, any more would be invalid)
            if (board_changes.num_changes == 0)
            {
                board_changes.presence_change_index_1 = i;
            }
            else if (board_changes.num_changes == 1)
            {
                board_changes.presence_change_index_2 = i;
            }
            else if (board_changes.num_changes == 2)
            {
                board_changes.presence_change_index_3 = i;
            }
            else if (board_changes.num_changes == 3)
            {
                board_changes.presence_change_index_4 = i;
            }

            // Increment the number of changes
            board_changes.num_changes += 1;
        }
    }

    return board_changes;
}

/**
 * @brief Determine the a move made (in UCI notation) by comparing two bit boards
 *
 * @param initial_presence The initial bit board
 * @param final_presence The final bit board
 * @param move A buffer to write the move into
 * @return Whether the move was (likely) legal (will know for sure once checked by the chess engine)
 */
static bool chessboard_get_move_from_presence(uint64_t initial_presence, uint64_t final_presence, char move[5])
{
    // Get the raw changes
    uint64_t presence_changes = (initial_presence ^ final_presence);

    // Get the indices of the changes
    board_changes_t board_changes = chessboard_get_board_changes_from_presence(initial_presence, final_presence);

    // Translate the changes to a move, and gently check legality
    bool legality = false;
    if (board_changes.num_changes == 2)             // Non-special move
    {
        char tile_initial[2];
        char tile_final[2];
        uint8_t initial_index = 0xFF;
        uint8_t index_a = board_changes.presence_change_index_1;
        uint8_t index_b = board_changes.presence_change_index_2;

        // Determine which index was the initial tile
        if ((initial_presence >> index_a) & 0x01)   // If index_a was 1 on the initial board, a piece moved from there
        {
            chessboard_presence_index_to_tile_buffer(index_a, tile_initial);
            chessboard_presence_index_to_tile_buffer(index_b, tile_final);
            initial_index = index_a;
        }
        else                                        // Otherwise, the piece moved to index_a
        {
            chessboard_presence_index_to_tile_buffer(index_a, tile_final);
            chessboard_presence_index_to_tile_buffer(index_b, tile_initial);
            initial_index = index_b;
        }

        // Fill the move buffer
        move[0] = tile_initial[0];
        move[1] = tile_initial[1];
        move[2] = tile_final[0];
        move[3] = tile_final[1];

        // Determine if this move was a promotion based on the current board
        uint8_t initial_file_index = chessboard_presence_index_to_file_index(initial_index);
        uint8_t initial_rank_index = chessboard_presence_index_to_rank_index(initial_index);
        char moving_piece = p_curr_board->board_pieces[initial_rank_index][initial_file_index];

        if (chessboard_is_promotion(tile_initial[1], tile_final[1], moving_piece))
        {
            move[4] = 'Q';
        }
        else
        {
            move[4] = '_';
        }

        // Mark move gently legal
        legality = true;
    }
    else if (board_changes.num_changes == 4)        // Castling move, check the possible signatures
    {
        switch (presence_changes)
        {
            case CASTLE_WHITE_K:
                move[0] = 'e';
                move[1] = '1';
                move[2] = 'g';
                move[3] = '1';
                move[4] = 'c';
                legality = true;
            break;

            case CASTLE_WHITE_Q:
                move[0] = 'e';
                move[1] = '1';
                move[2] = 'c';
                move[3] = '1';
                move[4] = 'c';
                legality = true;
            break;

            case CASTLE_BLACK_K:
                move[0] = 'e';
                move[1] = '8';
                move[2] = 'g';
                move[3] = '8';
                move[4] = 'c';
                legality = true;
            break;

            case CASTLE_BLACK_Q:
                move[0] = 'e';
                move[1] = '8';
                move[2] = 'c';
                move[3] = '8';
                move[4] = 'c';
                legality = true;
            break;

            default:
                // If none of the castling signatures matches, the move must have been illegal
                legality = false;
            break;
        }
    }
    else
    {
        // If the number of changes is not 2 or 4, the move must have been illegal
        legality = false;
    }

    return legality;
}

/**
 * @brief Determine the difference between two bit boards that correspond to a move made (in UCI notation)
 * 
 * @param initial_presence The initial bit board
 * @param move The move in UCI notation (4-5 characters)
 * @return The updated bit board
 */
static uint64_t chessboard_get_presence_from_move(uint64_t initial_presence, char move[5])
{
    uint8_t clear_presence_index = 0;
    uint8_t set_presence_index = 0;
    uint64_t final_presence = initial_presence;

    // Get the indices being changed
    switch (move[4])
    {
        case 'Q':
            // Promotion case
            clear_presence_index = chessboard_tile_to_presence_index(move[0], move[1]);
            set_presence_index = chessboard_tile_to_presence_index(move[2], move[3]);

            // Clear source, set dest
            final_presence &= ~(1 << clear_presence_index);
            final_presence |= (1 << set_presence_index);
        break;

        case 'C':
            // Capture case
            clear_presence_index = chessboard_tile_to_presence_index(move[0], move[1]);
            set_presence_index = chessboard_tile_to_presence_index(move[2], move[3]);

            // Clear source, but do not set the dest
            final_presence &= ~(1 << clear_presence_index);
        break;

        case 'c':
            // Castle case
            clear_presence_index = chessboard_tile_to_presence_index(move[0], move[1]);
            set_presence_index = chessboard_tile_to_presence_index(move[2], move[3]);

            // Clear king source, and set king dest
            final_presence &= ~(1 << clear_presence_index);
            final_presence |= (1 << set_presence_index);

            // Get the corresponding rook move
            char rook_move[5];
            chessboard_castle_get_rook_move(move, rook_move);

            // Clear rook source, and set rook dest
            clear_presence_index = chessboard_tile_to_presence_index(rook_move[0], rook_move[1]);
            set_presence_index = chessboard_tile_to_presence_index(rook_move[2], rook_move[3]);
            final_presence &= ~(1 << clear_presence_index);
            final_presence |= (1 << set_presence_index);
        break;

        case 'E':
            // En passent case, the captured pawn will have the moving pawn's *source rank* and *destination file*
            clear_presence_index = chessboard_tile_to_presence_index(move[0], move[3]);
            set_presence_index = chessboard_tile_to_presence_index(move[2], move[3]);

            // Clear source, set dest
            final_presence &= ~(1 << clear_presence_index);
            final_presence |= (1 << set_presence_index);
        break;

        case '_':
            // Standard move case
            clear_presence_index = chessboard_tile_to_presence_index(move[0], move[1]);
            set_presence_index = chessboard_tile_to_presence_index(move[2], move[3]);

            // Clear source, set dest
            final_presence &= ~(1 << clear_presence_index);
            final_presence |= (1 << set_presence_index);
        break;

        default:
            // Invalid move, do nothing
        break;
    }
    
    return final_presence;
}

/**
 * @brief Updates a board's pieces based on a move
 *
 * @param p_board The chessboard to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_pieces_from_move_activity(chess_board_t *p_board, char move[5])
{
    // Get the indices for the tiles in the move
    uint8_t move_initial_index = chessboard_tile_to_presence_index(move[0], move[1]);
    uint8_t move_final_index   = chessboard_tile_to_presence_index(move[2], move[3]);

    // Convert these indices to indices in the board_pieces 2D array
    uint8_t move_initial_file_index = chessboard_presence_index_to_file_index(move_initial_index);
    uint8_t move_initial_rank_index = chessboard_presence_index_to_rank_index(move_initial_index);
    uint8_t move_final_file_index   = chessboard_presence_index_to_file_index(move_final_index);
    uint8_t move_final_rank_index   = chessboard_presence_index_to_rank_index(move_final_index);

    // Clear the initial position
    char moving_piece = p_board->board_pieces[move_initial_rank_index][move_initial_file_index];
    p_board->board_pieces[move_initial_rank_index][move_initial_file_index] = '\0';

    // Update the final position, and account for promotion
    p_board->board_pieces[move_final_rank_index][move_final_file_index] = moving_piece;
    if ((move[4] == 'Q') || (move[4] == 'q'))
    {
        // 'p' + 1 = 'q' and 'P' + 1 = 'Q', so this works for both colors
        p_board->board_pieces[move_final_rank_index][move_final_file_index] += 1;
    }
}

/**
 * @brief Update an entire board's pieces from a move
 *
 * @param board The chessboard to apply the move to
 * @param move The move in UCI notation (4-5 characters)
 */
static void chessboard_update_pieces_from_move(chess_board_t *board, char move[5], bool human_move)
{
    // Check for castling move to update the board with the rook's move as well
    if (move[4] == 'c')
    {
        char rook_move[5];
        chessboard_castle_get_rook_move(move, rook_move);
        chessboard_update_pieces_from_move_activity(board, rook_move);
        
        // Our communication protocol uses a '_' for the human move when castling, rather than a 'c'
        if (human_move)
        {
            move[4] = '_';
        }
    }

    // Update for the specified move
    chessboard_update_pieces_from_move_activity(board, move);
}

/**
 * @brief Update a chessboard struct from its presence
 * 
 * @param p_board The board to update
 * @param new_presence How the board has changed
 * @param move Buffer to store move in UCI notation (4-5 characters)
 * @return Whether the move was legal
 */
static bool chessboard_update_from_presence(chess_board_t* p_board, uint64_t new_presence, char move[5])
{
    // Update the presence
    uint64_t old_presence = p_board->board_presence;
    p_board->board_presence = new_presence;

    // Update the pieces
    bool move_legal = chessboard_get_move_from_presence(old_presence, new_presence, move);
    chessboard_update_pieces_from_move(p_board, move, true);
    return move_legal;
}

/**
 * @brief Update a chessboard struct from its presence
 * 
 * @param p_board The board to update
 * @param new_presence How the board has changed
 * @param move Buffer to store move in UCI notation (4-5 characters)
 * @return Whether the move was legal
 */
static void chessboard_update_from_move(chess_board_t* p_board, char move[5])
{
    // Update the pieces
    chessboard_update_pieces_from_move(p_board, move, false);

    // Update the presence
    uint64_t old_presence = p_board->board_presence;
    uint64_t new_presence = chessboard_get_presence_from_move(old_presence, move);
    p_board->board_presence = new_presence;
}

/**
 * @brief Helper function to copy one board state to another. Used to reset intermedaite to previous or current to intermediate
 * 
 * @param p_source_board The board to copy from
 * @param p_dest_board The board to copy to
 */
static void chessboard_copy_board(chess_board_t* p_source_board, chess_board_t* p_dest_board)
{
    // Copy the presence
    p_dest_board->board_presence = p_source_board->board_presence;

    // Copy the pices
    uint8_t i = 0;
    uint8_t j = 0;
    for (i = 0; i < 64; i++)
    {
        for (j = 0; j < 64; j++)
        {
            p_dest_board->board_pieces[i][j] = p_source_board->board_pieces[i][j];
        }
    }
}

/**
 * @brief Gets a piece on the current board based on the file and rank
 * 
 * @param file The column to check
 * @param rank The row to check
 * @return chess_piece_t Which piece is there
 */
chess_piece_t chessboard_get_piece_at_position(chess_file_t file, chess_rank_t rank)
{
    // Get the indices of this position
    uint8_t presence_index = utils_tile_to_index(file, rank);
    uint8_t file_index = chessboard_presence_index_to_file_index(presence_index);
    uint8_t rank_index = chessboard_presence_index_to_rank_index(presence_index);

    // Get the piece at this position
    char piece = p_curr_board->board_pieces[rank_index][file_index];

    // Translate to the chess_piece type
    return utils_byte_to_piece_type(piece);
}

/**
 * @brief Public function to update the intermediate board
 * 
 * @param board_reading A board reading
 * @param move Buffer to store move in UCI notation (4-5 characters)
 * @return Whether the move was legal
 */
bool chessboard_update_intermediate_board_from_presence(uint64_t board_reading, char move[5])
{
    // Reset the intermediate board to the previous boad
    chessboard_copy_board(p_prev_board, p_inter_board);

    // Update the intermediate board from the given reading
    return chessboard_update_from_presence(p_inter_board, board_reading, move);
}

/**
 * @brief Public function to update the current board
 * 
 * @param board_reading A board reading
 * @param move Buffer to store move in UCI notation (4-5 characters)
 * @param capture Whether a capture occured (changes which board to update from)
 * @return Whether the move was legal
 */
bool chessboard_update_current_board_from_presence(uint64_t board_reading, char move[5], bool capture)
{
    // Reset the current board to the intermediate boad if there was a capture, otherwise reset to previous board
    if (capture)
    {
        chessboard_copy_board(p_inter_board, p_curr_board);
    } 
    else 
    {
        chessboard_copy_board(p_prev_board, p_curr_board);
    }

    // Update the current board from the given reading
    return chessboard_update_from_presence(p_curr_board, board_reading, move);
}

/**
 * @brief Public function to update the previous board once the human move is known to be legal
 */
void chessboard_update_previous_board_from_current_board(void)
{
    chessboard_copy_board(p_curr_board, p_prev_board);
}

/**
 * @brief Public function to update the previous board once the robot move is complete
 * 
 * @param move The move in UCI notation (4-5 characters)
 */
void chessboard_update_previous_board_from_move(char move[5])
{
    chessboard_update_from_move(p_prev_board, move);
}

/* End chessboard.c */
