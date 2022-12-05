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
void sensornetwork_select_tile(chess_file_t file, chess_rank_t rank);
static uint64_t sensornetwork_shift_assign(void);

// Declare the sensor lines
sensornetwork_line_t sensor_lines[NUMBER_OF_SENSOR_ROW_SELECTS + NUMBER_OF_SENSOR_COL_SELECTS];
static sensornetwork_line_t* sensor_row_select_1 = &sensor_lines[0];
static sensornetwork_line_t* sensor_row_select_2 = &sensor_lines[1];
static sensornetwork_line_t* sensor_row_select_3 = &sensor_lines[2];
static sensornetwork_line_t* sensor_col_select_1 = &sensor_lines[3];
static sensornetwork_line_t* sensor_col_select_2 = &sensor_lines[4];
static sensornetwork_line_t* sensor_col_select_3 = &sensor_lines[5];

// Declare the sensor that holds the readings
static sensornetwork_state_t sensors;
static sensornetwork_state_t* p_sensors = (&sensors);

/**
 * @brief Initialize the sensor select and data lines
 */
void sensornetwork_init(void)
{
    // Configure sensor row selects
    gpio_set_as_output(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
    gpio_set_as_output(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
    gpio_set_as_output(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);

    sensor_row_select_1->line_port = SENSOR_ROW_SELECT_0_PORT;
    sensor_row_select_1->line_pin  = SENSOR_ROW_SELECT_0_PIN;
    sensor_row_select_2->line_port = SENSOR_ROW_SELECT_1_PORT;
    sensor_row_select_2->line_pin  = SENSOR_ROW_SELECT_1_PIN;
    sensor_row_select_3->line_port = SENSOR_ROW_SELECT_2_PORT;
    sensor_row_select_3->line_pin  = SENSOR_ROW_SELECT_2_PIN;

    // Configure sensor column selects
    gpio_set_as_output(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
    gpio_set_as_output(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_as_output(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);

    sensor_col_select_1->line_port = SENSOR_COL_SELECT_0_PORT;
    sensor_col_select_1->line_pin  = SENSOR_COL_SELECT_0_PIN;
    sensor_col_select_2->line_port = SENSOR_COL_SELECT_1_PORT;
    sensor_col_select_2->line_pin  = SENSOR_COL_SELECT_1_PIN;
    sensor_col_select_3->line_port = SENSOR_COL_SELECT_2_PORT;
    sensor_col_select_3->line_pin  = SENSOR_COL_SELECT_2_PIN;

    // Configure data lines
    gpio_set_as_input(SENSOR_DATA_PORT, SENSOR_DATA_PIN);

    // Start the ISR timer
    clock_start_timer(SENSOR_NETWORK_TIMER);
}

/**
 * @brief Selects a tile to read
 * 
 * @param file The column to select
 * @param rank The row to select
 */
void sensornetwork_select_tile(chess_file_t file, chess_rank_t rank)
{
    // Set the row select lines
    switch (file)
    {
        case A: // Select == 000
            gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case B: // Select == 001
            gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case C: // Select == 010
            gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case D: // Select == 011
            gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case E: // Select == 100
            gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case F: // Select == 101
            gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case G: // Select == 110
            gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        case H: // Select == 111
            gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
            gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
            gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
        break;

        default: // ??
        break;
    }

    // Set the column select lines
    switch (rank)
    {
        case FIRST: // Select == 000
            gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case SECOND: // Select == 001
            gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case THIRD: // Select == 010
            gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case FOURTH: // Select == 011
            gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case FIFTH: // Select == 100
            gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case SIXTH: // Select == 101
            gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case SEVENTH: // Select == 110
            gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        case EIGHTH: // Select == 111
            gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
            gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
            gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
        break;

        default: // ??
        break;
    }

}

/**
 * @brief Gets the current sensor network reading
 * 
 * @return uint64_t The sensor readings
 */
uint64_t sensornetwork_get_reading(void)
{
    return p_sensors->current_inputs;
}

/* Interrupts */

/**
 * @brief Shifts all sensor network bits to a local ordering
 * 
 * @return The reassigned value for the sensors locally
 */
static uint64_t sensornetwork_shift_assign(void)
{
    // Loop through all tiles
    int i = 0;
    int j = 0;
    sensor_vport.image = 0;

    for (i = 0; i < NUMBER_OF_ROWS; i++)
    {
        for (j = 0; j < NUMBER_OF_COLS; j++)
        {
            // Select the tile to read
            chess_rank_t rank = utils_index_to_rank(i);
            chess_file_t file = utils_index_to_file(j);
            sensornetwork_select_tile(file, rank);

            // Read the sensor into the vport image
            sensor_vport.image |= (gpio_read_input(SENSOR_DATA_PORT, SENSOR_DATA_PIN) << utils_tile_to_index(file, rank));
        }
    }

    return sensor_vport.image;
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SENSOR_NETWORK_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(SENSOR_NETWORK_TIMER);

    // Read the sensors into the vport image
//    sensor_vport.image         = sensornetwork_shift_assign();

    // Update the sensor transition information
    p_sensors->current_inputs  = sensor_vport.image;
    p_sensors->edges           = (p_sensors->current_inputs ^ p_sensors->previous_inputs);
    p_sensors->pos_transitions = (p_sensors->current_inputs & p_sensors->edges);
    p_sensors->neg_transitions = ((~p_sensors->current_inputs) & p_sensors->edges);
    p_sensors->previous_inputs = p_sensors->current_inputs;
}

/* End sensornetwork.c */
