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

#define XBEE_ATTN_PORT GPIOC
#define XBEE_ATTN_PIN  GPIO_PIN_12


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

void xbee_transmit_minimal(const char *data, uint8_t data_len) {
    uint8_t frame[150];
    uint16_t frame_len = 14 + data_len;

    frame[0] = 0x7E;                     // Start delimiter
    frame[1] = (frame_len >> 8) & 0xFF;  // Length MSB
    frame[2] = frame_len & 0xFF;         // Length LSB

    frame[3] = 0x10;    // Frame type: TX Request
    frame[4] = 0x01;    // Frame ID

    // Destination 64-bit address (unicast to other XBee)
    frame[5]  = 0x00;   // SH byte 1
    frame[6]  = 0x13;   // SH byte 2
    frame[7]  = 0xA2;   // SH byte 3
    frame[8]  = 0x00;   // SH byte 4
    frame[9]  = 0x41;   // SL byte 1
    frame[10] = 0xC2;   // SL byte 2
    frame[11] = 0x69;   // SL byte 3
    frame[12] = 0x0F;   // SL byte 4

    // 16-bit address = unknown (let XBee resolve it)
    frame[13] = 0xFF;
    frame[14] = 0xFE;

    frame[15] = 0x00; // Broadcast radius
    frame[16] = 0x00; // Options

    memcpy(&frame[17], data, data_len);

    // Checksum
    uint8_t sum = 0;
    for (int i = 3; i < 17 + data_len; i++) {
        sum += frame[i];
    }
    frame[17 + data_len] = 0xFF - sum;

    // Transmit
    xbee_select();
    HAL_SPI_Transmit(&hspi3, frame, 18 + data_len, HAL_MAX_DELAY);
    xbee_deselect();
}


uint8_t FEB_xbee_receive_status(void) {
    uint8_t rx_buf[32] = {0};

    xbee_select();

    for (int i = 0; i < sizeof(rx_buf); i++) {
        uint8_t dummy = 0x00;
        HAL_SPI_TransmitReceive(&hspi3, &dummy, &rx_buf[i], 1, HAL_MAX_DELAY);
    }

    xbee_deselect();

    for (int i = 0; i < sizeof(rx_buf) - 5; i++) {
        if (rx_buf[i] == 0x7E && rx_buf[i + 3] == 0x8B) {
            uint8_t delivery_status = rx_buf[i + 8];  // 0x00 = success
            return (delivery_status == 0x00) ? 1 : 0;
        }
    }

    return 0;
}

////----CAN FRAME TRANSMIT----//
//void FEB_xbee_transmit_can_data(circBuffer *cb)
//{
//    // Check if buffer is empty
//    if (cb->count == 0) {
////        HAL_UART_Transmit(&huart2, (uint8_t*)"Error! No CAN data to send.\r\n", 29, HAL_MAX_DELAY);
//        return;
//    }
//
//    // Build CAN message string (excluding newline for cleaner formatting)
//    char can_message[128];
//    int message_len = snprintf(can_message, sizeof(can_message),
//                       "--Message-- Time(ms): %lu, ID: %u, Data:",
//                       (unsigned long)cb->buffer[cb->read].timestamp,
//                       cb->buffer[cb->read].id);
//
//    for (int i = 0; i < 8; i++) {
//        message_len += snprintf(can_message + message_len, sizeof(can_message) - message_len, " %02X",
//                                cb->buffer[cb->read].data[i]);
//    }
//
//    cb->read = (cb->read + 1) % cb->capacity;
//    cb->count--;
//
//    // Send using the new minimal frame function
//    xbee_transmit_minimal(can_message, message_len);
//}


void FEB_xbee_transmit_can_data(circBuffer *cb)
{
    if (cb->count == 0) return;

    // Build binary CAN frame (timestamp + ID + 8 bytes data + checksum = 17 bytes)
    uint8_t payload[17];
    size_t pos = 0;

    // 1. Timestamp (4 bytes, big-endian)
    uint32_t ts = cb->buffer[cb->read].timestamp;
    payload[pos++] = (ts >> 24) & 0xFF;
    payload[pos++] = (ts >> 16) & 0xFF;
    payload[pos++] = (ts >> 8)  & 0xFF;
    payload[pos++] = (ts)       & 0xFF;

    // 2. CAN ID (4 bytes, big-endian)
    uint32_t id = cb->buffer[cb->read].id;
    payload[pos++] = (id >> 24) & 0xFF;
    payload[pos++] = (id >> 16) & 0xFF;
    payload[pos++] = (id >> 8)  & 0xFF;
    payload[pos++] = (id)       & 0xFF;

    // 3. Data (8 bytes)
    memcpy(&payload[pos], cb->buffer[cb->read].data, 8);
    pos += 8;

    // 4. Simple XOR checksum
    uint8_t checksum = 0;
    for (size_t i = 0; i < pos; i++) {
        checksum ^= payload[i];
    }
    payload[pos++] = checksum;

    // Advance circular buffer
    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;

    // Send using your XBee SPI transmit function
    xbee_transmit_minimal((const char *)payload, pos);
}










void FEB_xbee_transmit_hello(void) {
    const char *msg = "hello";
    xbee_transmit_minimal(msg, strlen(msg));
}
