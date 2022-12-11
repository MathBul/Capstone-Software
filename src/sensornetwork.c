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
static void sensornetwork_select_file(chess_file_t file);
static uint8_t sensornetwork_read_rank(chess_rank_t rank);
static uint64_t sensornetwork_shift_assign(void);

// Declare the sensor that holds the readings
static sensornetwork_state_t sensors;
static sensornetwork_state_t* p_sensors = (&sensors);

/**
 * @brief Initialize the sensor select and data lines
 */
void sensornetwork_init(void)
{
    // Configure sensor column selects
    gpio_set_as_output(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
    gpio_set_as_output(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_as_output(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);

    // Configure sensor row lines
    gpio_set_as_input(SENSOR_ROW_DATA_1_PORT, SENSOR_ROW_DATA_1_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_2_PORT, SENSOR_ROW_DATA_2_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_3_PORT, SENSOR_ROW_DATA_3_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_4_PORT, SENSOR_ROW_DATA_4_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_5_PORT, SENSOR_ROW_DATA_5_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_6_PORT, SENSOR_ROW_DATA_6_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_7_PORT, SENSOR_ROW_DATA_7_PIN);
    gpio_set_as_input(SENSOR_ROW_DATA_8_PORT, SENSOR_ROW_DATA_8_PIN);

    // Theory
    gpio_set_as_output(GPIOM, GPIO_PIN_7);
    gpio_set_as_output(GPIOP, GPIO_PIN_5);
    gpio_set_as_output(GPIOA, GPIO_PIN_7);
    gpio_set_output_low(GPIOM, GPIO_PIN_7);
    gpio_set_output_low(GPIOP, GPIO_PIN_5);
    gpio_set_output_low(GPIOA, GPIO_PIN_7);

    gpio_set_as_input(GPIOG, GPIO_PIN_1);


    // Start the ISR timer
    clock_start_timer(SENSOR_NETWORK_TIMER);
}

/**
 * @brief Selects a given tile to read
 * 
 * @param file The column to select
 */
static void sensornetwork_select_file(chess_file_t file)
{
    // Set the row select lines
    switch (file)
    {
        case A: // Select == 001
            gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case B: // Select == 000
            gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case C: // Select == 010
            gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case D: // Select == 011
            gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case E: // Select == 100
            gpio_set_output_high(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case F: // Select == 101
            gpio_set_output_high(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case G: // Select == 111
            gpio_set_output_high(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        case H: // Select == 110
            gpio_set_output_high(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
            gpio_set_output_high(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
            gpio_set_output_low(SENSOR_COL_SELECT_0_PORT, SENSOR_COL_SELECT_0_PIN);
        break;

        default: // ??
        break;
    }
}

/**
 * @brief Reads a given row
 *
 * @param rank The row to select
 * @return The reading of the given tile
 */
static uint8_t sensornetwork_read_rank(chess_rank_t rank)
{
    uint8_t reading = 0;

    // Set the column select lines
    switch (rank)
    {
        case FIRST: // Select == 000
            reading = gpio_read_input(SENSOR_ROW_DATA_1_PORT, SENSOR_ROW_DATA_1_PIN);
        break;

        case SECOND: // Select == 001
            reading = gpio_read_input(SENSOR_ROW_DATA_2_PORT, SENSOR_ROW_DATA_2_PIN);
        break;

        case THIRD: // Select == 010
            reading = gpio_read_input(SENSOR_ROW_DATA_3_PORT, SENSOR_ROW_DATA_3_PIN);
        break;

        case FOURTH: // Select == 011
            reading = gpio_read_input(SENSOR_ROW_DATA_4_PORT, SENSOR_ROW_DATA_4_PIN);
        break;

        case FIFTH: // Select == 100
            reading = gpio_read_input(SENSOR_ROW_DATA_5_PORT, SENSOR_ROW_DATA_5_PIN);
        break;

        case SIXTH: // Select == 101
            reading = gpio_read_input(SENSOR_ROW_DATA_6_PORT, SENSOR_ROW_DATA_6_PIN);
        break;

        case SEVENTH: // Select == 110
            reading = gpio_read_input(SENSOR_ROW_DATA_7_PORT, SENSOR_ROW_DATA_7_PIN);
        break;

        case EIGHTH: // Select == 111
            reading = gpio_read_input(SENSOR_ROW_DATA_8_PORT, SENSOR_ROW_DATA_8_PIN);
        break;

        default: // ??
        break;
    }

    return reading;
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
    chess_file_t file = A;
    chess_rank_t rank = FIRST;

    // Loop through all tiles
    int i = 0;
    int j = 0;
    for (i = 0; i < NUMBER_OF_COLS; i++)
    {
        // Select the file
        file = utils_index_to_file(i);
        sensornetwork_select_file(file);

        for (j = 0; j < NUMBER_OF_ROWS; j++)
        {
            utils_delay(300);

            // Read the rank
            rank = utils_index_to_rank(j);
            sensor_reading |= (((uint64_t) sensornetwork_read_rank(rank)) << utils_tile_to_index(file, rank));
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
    sensor_vport.image = sensornetwork_shift_assign();

    // Update the sensor transition information
    p_sensors->current_inputs  = sensor_vport.image;
    p_sensors->edges           = (p_sensors->current_inputs ^ p_sensors->previous_inputs);
    p_sensors->pos_transitions = (p_sensors->current_inputs & p_sensors->edges);
    p_sensors->neg_transitions = ((~p_sensors->current_inputs) & p_sensors->edges);
    p_sensors->previous_inputs = p_sensors->current_inputs;
}

/* End sensornetwork.c */
