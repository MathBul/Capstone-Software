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
uint8_t sensornetwork_read_tile(chess_file_t file, chess_rank_t rank);
static uint64_t sensornetwork_shift_assign(void);

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

    // Configure sensor column lines
    gpio_set_as_input(SENSOR_COL_DATA_0_PORT, SENSOR_COL_DATA_0_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_1_PORT, SENSOR_COL_DATA_1_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_2_PORT, SENSOR_COL_DATA_2_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_3_PORT, SENSOR_COL_DATA_3_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_4_PORT, SENSOR_COL_DATA_4_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_5_PORT, SENSOR_COL_DATA_5_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_6_PORT, SENSOR_COL_DATA_6_PIN);
    gpio_set_as_input(SENSOR_COL_DATA_7_PORT, SENSOR_COL_DATA_7_PIN);

    // Start the ISR timer
    clock_start_timer(SENSOR_NETWORK_TIMER);
}

/**
 * @brief Reads a given tile
 * 
 * @param file The column to select
 * @param rank The row to select
 * @return The reading of the given tile
 */
uint8_t sensornetwork_read_tile(chess_file_t file, chess_rank_t rank)
{
    uint8_t sensor_reading = 0;

    // Set the row select lines
    switch (file)
    {
        case A: // Select == 000
            gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case B: // Select == 001
            gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case C: // Select == 010
            gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case D: // Select == 011
            gpio_set_output_low(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case E: // Select == 100
            gpio_set_output_high(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case F: // Select == 101
            gpio_set_output_high(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case G: // Select == 110
            gpio_set_output_high(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        case H: // Select == 111
            gpio_set_output_high(SENSOR_ROW_SELECT_2_PORT, SENSOR_ROW_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_1_PORT, SENSOR_ROW_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_ROW_SELECT_0_PORT, SENSOR_ROW_SELECT_0_PIN);
        break;

        default: // ??
        break;
    }

    // Set the column select lines
    switch (rank)
    {
        case FIRST: // Select == 000
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_0_PORT, SENSOR_COL_DATA_0_PIN);
        break;

        case SECOND: // Select == 001
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_1_PORT, SENSOR_COL_DATA_1_PIN);
        break;

        case THIRD: // Select == 010
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_2_PORT, SENSOR_COL_DATA_2_PIN);
        break;

        case FOURTH: // Select == 011
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_3_PORT, SENSOR_COL_DATA_3_PIN);
        break;

        case FIFTH: // Select == 100
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_4_PORT, SENSOR_COL_DATA_4_PIN);
        break;

        case SIXTH: // Select == 101
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_5_PORT, SENSOR_COL_DATA_5_PIN);
        break;

        case SEVENTH: // Select == 110
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_6_PORT, SENSOR_COL_DATA_6_PIN);
        break;

        case EIGHTH: // Select == 111
            sensor_reading = gpio_read_input(SENSOR_COL_DATA_7_PORT, SENSOR_COL_DATA_7_PIN);
        break;

        default: // ??
        break;
    }

    return sensor_reading;
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
    uint64_t sensor_reading = 0;

    // Loop through all tiles
    int i = 0;
    int j = 0;
    for (i = 0; i < NUMBER_OF_ROWS; i++)
    {
        for (j = 0; j < NUMBER_OF_COLS; j++)
        {
            // Select the tile
            chess_rank_t rank = utils_index_to_rank(i);
            chess_file_t file = utils_index_to_file(j);

            // Read the tile
            sensor_reading |= (sensornetwork_read_tile(file, rank) << utils_tile_to_index(file, rank));
        }
    }

    return sensor_reading;
}

/**
 * @brief Interrupt handler for the switch module
 */
__interrupt void SENSOR_NETWORK_HANDLER(void)
{
    // Clear the interrupt flag
    clock_clear_interrupt(SENSOR_NETWORK_TIMER);

    // Read the sensors into the vport image
    sensor_vport.image         = sensornetwork_shift_assign();

    // Update the sensor transition information
    p_sensors->current_inputs  = sensor_vport.image;
    p_sensors->edges           = (p_sensors->current_inputs ^ p_sensors->previous_inputs);
    p_sensors->pos_transitions = (p_sensors->current_inputs & p_sensors->edges);
    p_sensors->neg_transitions = ((~p_sensors->current_inputs) & p_sensors->edges);
    p_sensors->previous_inputs = p_sensors->current_inputs;
}

/* End sensornetwork.c */
