/*
 * FEB_XBee.c
 *
 *  Created on: Feb 1, 2025
 *      Author: mihirtakalkar
 */

#include "FEB_XBee.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;

// CHIP SELECT LINES
#define XBEE_CS_PORT GPIOA
#define XBEE_CS_PIN  GPIO_PIN_15

// CS Low (Start SPI Message)
static void xbee_select(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_RESET);
}

// CS High (End SPI Message)
static void xbee_deselect(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_SET);
}

//----TRANSMIT FUNCTION----//
void xbee_transmit(const char *message)
{
    xbee_select(); // CS low
    HAL_SPI_Transmit(&hspi3, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    xbee_deselect(); // CS high
}

//----TESTING LOOP----//
void FEB_xbee_loop()
{
    static uint32_t counter = 0;
    char message[50];
    snprintf(message, sizeof(message), "hello", (unsigned long)counter); // Transmits hello
    xbee_transmit_api_frame(0x01, message);
    HAL_UART_Transmit(&huart2, (uint8_t*)"SPI frame sent\r\n", 15, HAL_MAX_DELAY);


    counter++;
    HAL_Delay(500); // EDIT FOR TESTING
}

//----TRANSMIT API FRAME----//
void xbee_transmit_minimal(const char *data, uint8_t data_len)
{
    uint8_t frame[150]; // large enough for typical usage
    uint16_t frame_len = 14 + data_len; // 0x10 frame + data

    frame[0] = 0x7E;                     // Start delimiter
    frame[1] = (frame_len >> 8) & 0xFF;  // Length MSB
    frame[2] = frame_len & 0xFF;         // Length LSB

    frame[3] = 0x10;    // Frame type: TX Request
    frame[4] = 0x01;    // Frame ID

    // Broadcast address (64 bit)
    frame[5] = 0x00;
    frame[6] = 0x00;
    frame[7] = 0x00;
    frame[8] = 0x00;
    frame[9] = 0x00;
    frame[10] = 0x00;
    frame[11] = 0xFF;
    frame[12] = 0xFF;

    // 16-bit address
    frame[13] = 0xFF;
    frame[14] = 0xFE;

    frame[15] = 0x00; // Broadcast radius
    frame[16] = 0x00; // Options

    memcpy(&frame[17], data, data_len);

    // Checksum calculation
    uint8_t sum = 0;
    for (int i = 3; i < 17 + data_len; i++) {
        sum += frame[i];
    }
    frame[17 + data_len] = 0xFF - sum;

    // Transmit
    xbee_select();
    HAL_SPI_Transmit(&hspi3, frame, 18 + data_len, HAL_MAX_DELAY);
    xbee_deselect();

    HAL_UART_Transmit(&huart2, (uint8_t*)"Minimal frame sent\r\n", 19, HAL_MAX_DELAY);
}

//----CAN FRAME TRANSMIT----//
void FEB_xbee_transmit_can_data(circBuffer *cb)
{
    // Check if buffer is empty
    if (cb->count == 0) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error! No CAN data to send.\r\n", 29, HAL_MAX_DELAY);
        return;
    }

    // Build CAN message string (excluding newline for cleaner formatting)
    char can_message[128];
    int message_len = snprintf(can_message, sizeof(can_message),
                       "--Message-- Time(ms): %lu, ID: %u, Data:",
                       (unsigned long)cb->buffer[cb->read].timestamp,
                       cb->buffer[cb->read].id);

    for (int i = 0; i < 8; i++) {
        message_len += snprintf(can_message + message_len, sizeof(can_message) - message_len, " %02X",
                                cb->buffer[cb->read].data[i]);
    }

    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;

    // Send using the new minimal frame function
    xbee_transmit_minimal(can_message, message_len);
}
