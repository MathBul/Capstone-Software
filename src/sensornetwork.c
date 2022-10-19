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

// Declare the sensor lines
sensor_line_t sensor_lines[(NUMBER_OF_SENSOR_ROW_SELECTS+1) + (NUMBER_OF_SENSOR_COL_SELECTS+1)]; // +1 for data lines
static sensor_line_t* sensor_row_select_1 = &sensor_lines[0];
static sensor_line_t* sensor_row_select_2 = &sensor_lines[1];
static sensor_line_t* sensor_row_select_3 = &sensor_lines[2];
static sensor_line_t* sensor_row_data     = &sensor_lines[3];
static sensor_line_t* sensor_col_select_1 = &sensor_lines[4];
static sensor_line_t* sensor_col_select_2 = &sensor_lines[5];
static sensor_line_t* sensor_col_select_3 = &sensor_lines[6];
static sensor_line_t* sensor_col_data     = &sensor_lines[7];

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

    gpio_set_as_output(SENSOR_ROW_DATA_PORT, SENSOR_ROW_DATA_PIN);
    gpio_set_output_low(SENSOR_ROW_DATA_PORT, SENSOR_ROW_DATA_PIN);

    sensor_row_select_1->line_port = SENSOR_ROW_SELECT_1_PORT;
    sensor_row_select_1->line_pin  = SENSOR_ROW_SELECT_1_PIN;
    sensor_row_select_2->line_port = SENSOR_ROW_SELECT_2_PORT;
    sensor_row_select_2->line_pin  = SENSOR_ROW_SELECT_2_PIN;
    sensor_row_select_3->line_port = SENSOR_ROW_SELECT_3_PORT;
    sensor_row_select_3->line_pin  = SENSOR_ROW_SELECT_3_PIN;
    sensor_row_data->line_port     = SENSOR_ROW_DATA_PORT;
    sensor_row_data->line_pin      = SENSOR_ROW_DATA_PIN;


    /* Sensor columns */
    gpio_set_as_output(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_1_PORT, SENSOR_COL_SELECT_1_PIN);

    gpio_set_as_output(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_2_PORT, SENSOR_COL_SELECT_2_PIN);

    gpio_set_as_output(SENSOR_COL_SELECT_3_PORT, SENSOR_COL_SELECT_3_PIN);
    gpio_set_output_low(SENSOR_COL_SELECT_3_PORT, SENSOR_COL_SELECT_3_PIN);

    gpio_set_as_input(SENSOR_COL_DATA_PORT, SENSOR_COL_DATA_PIN);

    sensor_col_select_1->line_port = SENSOR_COL_SELECT_1_PORT;
    sensor_col_select_1->line_pin  = SENSOR_COL_SELECT_1_PIN;
    sensor_col_select_2->line_port = SENSOR_COL_SELECT_2_PORT;
    sensor_col_select_2->line_pin  = SENSOR_COL_SELECT_2_PIN;
    sensor_col_select_3->line_port = SENSOR_COL_SELECT_3_PORT;
    sensor_col_select_3->line_pin  = SENSOR_COL_SELECT_3_PIN;
    sensor_col_data->line_port     = SENSOR_COL_DATA_PORT;
    sensor_col_data->line_pin      = SENSOR_COL_DATA_PIN;
}

/**
 * @brief Read the specified tile
 * 
 * @param row The row to be read
 * @param col The column to be read
 * @return sensor_status_t Whether a piece is present or absent in the specified tile
 */
sensor_status_t sensors_read_tile(sensor_pos_t row, sensor_pos_t col)
{
    // Set the row select lines
    if (row == row_1) // Select == 000
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_2) // Select == 001
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_3) // Select == 010
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_4) // Select == 011
    {
        gpio_set_output_low(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_5) // Select == 100
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_6) // Select == 101
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_low(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_7) // Select == 110
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_low(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    } 
    else if (row == row_8) // Select == 111
    {
        gpio_set_output_high(sensor_row_select_3->line_port, sensor_row_select_3->line_pin);
        gpio_set_output_high(sensor_row_select_2->line_port, sensor_row_select_2->line_pin);
        gpio_set_output_high(sensor_row_select_1->line_port, sensor_row_select_1->line_pin);
    }

    // Set the column select lines
    if (col == col_a) // Select == 000
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_b) // Select == 001
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_c) // Select == 010
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_d) // Select == 011
    {
        gpio_set_output_low(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_e) // Select == 100
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_f) // Select == 101
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_low(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_g) // Select == 110
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_low(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    } 
    else if (col == col_h) // Select == 111
    {
        gpio_set_output_high(sensor_col_select_3->line_port, sensor_col_select_3->line_pin);
        gpio_set_output_high(sensor_col_select_2->line_port, sensor_col_select_2->line_pin);
        gpio_set_output_high(sensor_col_select_1->line_port, sensor_col_select_1->line_pin);
    }

    // Read the data
    gpio_set_output_high(sensor_row_data->line_port, sensor_row_data->line_pin);
    uint8_t switch_data = gpio_read_input(sensor_col_data->line_port, sensor_col_data->line_pin) ;

    // Produce the output
    if (switch_data == 1)
    {
        return present;
    }
    return absent;
}

/* End sensornetwork.c */
