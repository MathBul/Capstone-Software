/**
 * @file sensornetwork.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for the reed switch sensor network
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "sensornetwork.h"

// Private functions
void sensors_select_tile(chess_file_t file, chess_rank_t rank);
static uint64_t sensor_shift_assign(uint8_t port_raw, chess_file_t file, chess_rank_t rank);
static uint64_t sensor_tile_to_mask(chess_file_t file, chess_rank_t rank);

// Declare the sensor select lines
sensor_line_t sensor_lines[NUMBER_OF_SENSOR_ROW_SELECTS + NUMBER_OF_SENSOR_COL_SELECTS + NUMBER_OF_DATA_LINES];
static sensor_line_t* sensor_row_select_1 = &sensor_lines[0];
static sensor_line_t* sensor_row_select_2 = &sensor_lines[1];
static sensor_line_t* sensor_row_select_3 = &sensor_lines[2];
static sensor_line_t* sensor_col_select_1 = &sensor_lines[4];
static sensor_line_t* sensor_col_select_2 = &sensor_lines[5];
static sensor_line_t* sensor_col_select_3 = &sensor_lines[6];

// Declare the sensor itself
static sensor_state_t sensors;
static sensor_state_t* p_sensors = (&sensors);

/**
 * @brief Initialize the sensor select and data lines
 */
void sensors_init()
{
    /* Sensor rows */
    gpio_set_as_output(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);

    gpio_set_as_output(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);

    gpio_set_as_output(SENSOR_ROW_SELECT_3_PORT, SENSOR_ROW_SELECT_3_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_3_PORT, SENSOR_ROW_SELECT_3_PIN);

    sensor_row_select_1->line_port = SENSOR_ROW_SELECT_1_PORT;
    sensor_row_select_1->line_pin  = SENSOR_ROW_SELECT_1_PIN;
    sensor_row_select_2->line_port = SENSOR_ROW_SELECT_2_PORT;
    sensor_row_select_2->line_pin  = SENSOR_ROW_SELECT_2_PIN;
    sensor_row_select_3->line_port = SENSOR_ROW_SELECT_3_PORT;
    sensor_row_select_3->line_pin  = SENSOR_ROW_SELECT_3_PIN;


    /* Sensor columns */
    gpio_set_as_output(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);

    gpio_set_as_output(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);

    gpio_set_as_output(SENSOR_COL_SELECT_3_PORT, SENSOR_COL_SELECT_3_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_3_PORT, SENSOR_COL_SELECT_3_PIN);

    sensor_col_select_1->line_port = SENSOR_COL_SELECT_1_PORT;
    sensor_col_select_1->line_pin  = SENSOR_COL_SELECT_1_PIN;
    sensor_col_select_2->line_port = SENSOR_COL_SELECT_2_PORT;
    sensor_col_select_2->line_pin  = SENSOR_COL_SELECT_2_PIN;
    sensor_col_select_3->line_port = SENSOR_COL_SELECT_3_PORT;
    sensor_col_select_3->line_pin  = SENSOR_COL_SELECT_3_PIN;

    /* Data line(s) */
    gpio_set_as_input(SENSOR_DATA_PORT, SENSOR_DATA_PIN);
}

/**
 * @brief Selects a tile to read
 * 
 * @param file The column to select
 * @param rank The row to select
 */
void sensors_select_tile(chess_file_t file, chess_rank_t rank)
{
    // Set the row select lines
    if (file == A)          // Select == 000
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == B)     // Select == 001
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == C)     // Select == 010
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == D)     // Select == 011
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == E)     // Select == 100
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == F)     // Select == 101
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == G)     // Select == 110
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (file == H)     // Select == 111
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    }

    // Set the column select lines
    if (rank == FIRST)      // Select == 000
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == SECOND) // Select == 001
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == THIRD) // Select == 010
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == FOURTH) // Select == 011
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == FIFTH) // Select == 100
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == SIXTH) // Select == 101
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == SEVENTH) // Select == 110
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (rank == EIGHTH) // Select == 111
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    }
}

/**
 * @brief Gets the current sensor network reading
 * 
 * @return uint64_t The sensor readings
 */
uint64_t sensor_get_reading()
{
    return p_sensors->current_inputs;
}

/* Interrupts */

/**
 * @brief Helper function to translate a rank/file to a tile mask
 *
 * @param file The column to select
 * @param rank The row to select
 * @return uint64_t The tile mask
 */
static uint64_t sensor_tile_to_mask(chess_file_t file, chess_rank_t rank)
{
    uint64_t tile_mask = 0;

    // Determine the mask for this tile
    if ((file == A) && (rank == FIRST))
    {
        tile_mask = TILE_A1;
    } 
    else if ((file == A) && (rank == SECOND))
    {
        tile_mask = TILE_A2;
    } 
    else if ((file == A) && (rank == THIRD))
    {
        tile_mask = TILE_A3;
    } 
    else if ((file == A) && (rank == FOURTH))
    {
        tile_mask = TILE_A4;
    } 
    else if ((file == A) && (rank == FIFTH))
    {
        tile_mask = TILE_A5;
    } 
    else if ((file == A) && (rank == SIXTH))
    {
        tile_mask = TILE_A6;
    } 
    else if ((file == A) && (rank == SEVENTH))
    {
        tile_mask = TILE_A7;
    } 
    else if ((file == A) && (rank == EIGHTH))
    {
        tile_mask = TILE_A8;
    } 
    else if ((file == B) && (rank == FIRST))
    {
        tile_mask = TILE_B1;
    } 
    else if ((file == B) && (rank == SECOND))
    {
        tile_mask = TILE_B2;
    } 
    else if ((file == B) && (rank == THIRD))
    {
        tile_mask = TILE_B3;
    } 
    else if ((file == B) && (rank == FOURTH))
    {
        tile_mask = TILE_B4;
    } 
    else if ((file == B) && (rank == FIFTH))
    {
        tile_mask = TILE_B5;
    } 
    else if ((file == B) && (rank == SIXTH))
    {
        tile_mask = TILE_B6;
    } 
    else if ((file == B) && (rank == SEVENTH))
    {
        tile_mask = TILE_B7;
    } 
    else if ((file == B) && (rank == EIGHTH))
    {
        tile_mask = TILE_B8;
    } 
    else if ((file == C) && (rank == FIRST))
    {
        tile_mask = TILE_C1;
    } 
    else if ((file == C) && (rank == SECOND))
    {
        tile_mask = TILE_C2;
    } 
    else if ((file == C) && (rank == THIRD))
    {
        tile_mask = TILE_C3;
    } 
    else if ((file == C) && (rank == FOURTH))
    {
        tile_mask = TILE_C4;
    } 
    else if ((file == C) && (rank == FIFTH))
    {
        tile_mask = TILE_C5;
    } 
    else if ((file == C) && (rank == SIXTH))
    {
        tile_mask = TILE_C6;
    } 
    else if ((file == C) && (rank == SEVENTH))
    {
        tile_mask = TILE_C7;
    } 
    else if ((file == C) && (rank == EIGHTH))
    {
        tile_mask = TILE_C8;
    } 
    else if ((file == D) && (rank == FIRST))
    {
        tile_mask = TILE_D1;
    } 
    else if ((file == D) && (rank == SECOND))
    {
        tile_mask = TILE_D2;
    } 
    else if ((file == D) && (rank == THIRD))
    {
        tile_mask = TILE_D3;
    } 
    else if ((file == D) && (rank == FOURTH))
    {
        tile_mask = TILE_D4;
    } 
    else if ((file == D) && (rank == FIFTH))
    {
        tile_mask = TILE_D5;
    } 
    else if ((file == D) && (rank == SIXTH))
    {
        tile_mask = TILE_D6;
    } 
    else if ((file == D) && (rank == SEVENTH))
    {
        tile_mask = TILE_D7;
    } 
    else if ((file == D) && (rank == EIGHTH))
    {
        tile_mask = TILE_D8;
    } 
    else if ((file == E) && (rank == FIRST))
    {
        tile_mask = TILE_E1;
    } 
    else if ((file == E) && (rank == SECOND))
    {
        tile_mask = TILE_E2;
    } 
    else if ((file == E) && (rank == THIRD))
    {
        tile_mask = TILE_E3;
    } 
    else if ((file == E) && (rank == FOURTH))
    {
        tile_mask = TILE_E4;
    } 
    else if ((file == E) && (rank == FIFTH))
    {
        tile_mask = TILE_E5;
    } 
    else if ((file == E) && (rank == SIXTH))
    {
        tile_mask = TILE_E6;
    } 
    else if ((file == E) && (rank == SEVENTH))
    {
        tile_mask = TILE_E7;
    } 
    else if ((file == E) && (rank == EIGHTH))
    {
        tile_mask = TILE_E8;
    } 
    else if ((file == F) && (rank == FIRST))
    {
        tile_mask = TILE_F1;
    } 
    else if ((file == F) && (rank == SECOND))
    {
        tile_mask = TILE_F2;
    } 
    else if ((file == F) && (rank == THIRD))
    {
        tile_mask = TILE_F3;
    } 
    else if ((file == F) && (rank == FOURTH))
    {
        tile_mask = TILE_F4;
    } 
    else if ((file == F) && (rank == FIFTH))
    {
        tile_mask = TILE_F5;
    } 
    else if ((file == F) && (rank == SIXTH))
    {
        tile_mask = TILE_F6;
    } 
    else if ((file == F) && (rank == SEVENTH))
    {
        tile_mask = TILE_F7;
    } 
    else if ((file == F) && (rank == EIGHTH))
    {
        tile_mask = TILE_F8;
    } 
    else if ((file == G) && (rank == FIRST))
    {
        tile_mask = TILE_G1;
    } 
    else if ((file == G) && (rank == SECOND))
    {
        tile_mask = TILE_G2;
    } 
    else if ((file == G) && (rank == THIRD))
    {
        tile_mask = TILE_G3;
    } 
    else if ((file == G) && (rank == FOURTH))
    {
        tile_mask = TILE_G4;
    } 
    else if ((file == G) && (rank == FIFTH))
    {
        tile_mask = TILE_G5;
    } 
    else if ((file == G) && (rank == SIXTH))
    {
        tile_mask = TILE_G6;
    } 
    else if ((file == G) && (rank == SEVENTH))
    {
        tile_mask = TILE_G7;
    } 
    else if ((file == G) && (rank == EIGHTH))
    {
        tile_mask = TILE_G8;
    } 
    else if ((file == H) && (rank == FIRST))
    {
        tile_mask = TILE_H1;
    } 
    else if ((file == H) && (rank == SECOND))
    {
        tile_mask = TILE_H2;
    } 
    else if ((file == H) && (rank == THIRD))
    {
        tile_mask = TILE_H3;
    } 
    else if ((file == H) && (rank == FOURTH))
    {
        tile_mask = TILE_H4;
    } 
    else if ((file == H) && (rank == FIFTH))
    {
        tile_mask = TILE_H5;
    } 
    else if ((file == H) && (rank == SIXTH))
    {
        tile_mask = TILE_H6;
    } 
    else if ((file == H) && (rank == SEVENTH))
    {
        tile_mask = TILE_H7;
    } 
    else if ((file == H) && (rank == EIGHTH))
    {
        tile_mask = TILE_H8;
    } 

    return tile_mask;
}

/**
 * @brief Shifts all sensor network bits from a vport bitfield to a local ordering
 * 
 * @param port_raw The raw value read from the sensor image
 * @param file The column to select
 * @param rank The row to select
 * @return uint64_t The reassigned value for the sensors locally
 */
static uint64_t sensor_shift_assign(uint8_t port_raw, chess_file_t file, chess_rank_t rank)
{
    uint64_t sensors_reassigned = 0;
    uint64_t tile_mask = sensor_tile_to_mask(file, rank);

    // Move the bit the proper position
    sensors_reassigned |= utils_bits8_remask(port_raw, SENSOR_DATA_PIN, tile_mask);

    return sensors_reassigned;
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SENSOR_NETWORK_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(SENSOR_NETWORK_TIMER);

    // Arrays to convert indices to ranks/files
    chess_file_t files[NUMBER_OF_ROWS_COLS] = {A, B, C, D, E, F, G, H};
    chess_rank_t ranks[NUMBER_OF_ROWS_COLS] = {FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH, EIGHTH};

    // Loop through all tiles
    uint8_t col = 0;
    uint8_t row = 0;
    sensor_vport.image = 0;

    for (col = 0; col < NUMBER_OF_ROWS_COLS; col++)
    {
        for (row = 0; row < NUMBER_OF_ROWS_COLS; row++)
        {
            // Select the tile to read
            chess_file_t file = files[col];
            chess_rank_t rank = ranks[row];
            sensors_select_tile(file, rank);

            // Read the sensor into the vport image
            uint8_t sensors_raw         = gpio_read_input(SENSOR_DATA_PORT, SENSOR_DATA_PIN);
            sensor_vport.image          |= sensor_shift_assign(sensors_raw, file, rank);
        }
    }

    // Update the switch transition information
    p_sensors->current_inputs  = sensor_vport.image;
    p_sensors->edges           = (p_sensors->current_inputs ^ p_sensors->previous_inputs);
    p_sensors->pos_transitions = (p_sensors->current_inputs & p_sensors->edges);
    p_sensors->neg_transitions = ((~p_sensors->current_inputs) & p_sensors->edges);
    p_sensors->previous_inputs = p_sensors->current_inputs;
}

/* End sensornetwork.c */
