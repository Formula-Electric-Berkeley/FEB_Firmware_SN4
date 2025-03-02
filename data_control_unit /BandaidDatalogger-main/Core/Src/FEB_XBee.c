/*
 * FEB_XBee.c
 *
 *  Created on: Feb 1, 2025
 *      Author: mihirtakalkar
 */

#include "FEB_XBee.h"
#include <string.h>
#include <stdio.h>


extern SPI_HandleTypeDef hspi3;  // TODO: Edit based on IOC
extern UART_HandleTypeDef huart2;

// CHIP SELECT LINES
#define XBEE_CS_PORT GPIOB
#define XBEE_CS_PIN  GPIO_PIN_12

// CS Low (start)
static void xbee_select(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_RESET);
}

// CS High (end)
static void xbee_deselect(void)
{
    HAL_GPIO_WritePin(XBEE_CS_PORT, XBEE_CS_PIN, GPIO_PIN_SET);
}

/**
 * String transmit
 */
void xbee_transmit(const char *message)
{
    xbee_select(); // CS low
    HAL_SPI_Transmit(&hspi3, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    xbee_deselect(); // CS high
}

/**
 * Basic loop for testing
 */
void FEB_xbee_loop()
{
    uint32_t counter = 0;
    char message[50];
    snprintf(message, sizeof(message), "XBee TX: %lu\r\n", (unsigned long)counter);
    xbee_transmit(message);

    counter++;
    // HAL_Delay(1000); // editable
}

/**
 * Transmit CAN data from a circular buffer
 */
void FEB_xbee_transmit_can_data(circBuffer *cb)
{
    if (cb->count == 0) {
        // Debug/error message still using UART2
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error! No CAN data to send.\r\n", 29, HAL_MAX_DELAY);
        return;
    }

    char can_message[128];
    int len = snprintf(can_message, sizeof(can_message),
                       "CAN TX: Time(ms): %lu, ID: %u, Data:",
                       (unsigned long)cb->buffer[cb->read].timestamp,
                       cb->buffer[cb->read].id);

    for (int i = 0; i < 8; i++) {
        len += snprintf(can_message + len, sizeof(can_message) - len, " %02X",
                        cb->buffer[cb->read].data[i]);
    }
    strcat(can_message, "\r\n");

    xbee_transmit(can_message);

    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;
}

/**
 * Transmit the latest SD card data (SD card erros to UART2)
 */
//void FEB_xbee_transmit_sd_data()
//{
//    char sd_message[128];
//
//    // Move file pointer near the end to read last entry
//    fres = f_lseek(&fil, f_size(&fil) - sizeof(sd_message));
//    if (fres != FR_OK) {
//        HAL_UART_Transmit(&huart2, (uint8_t*)"Error finding last SD entry.\r\n", 30, HAL_MAX_DELAY);
//        return;
//    }
//
//    fres = f_read(&fil, sd_message, sizeof(sd_message), &br);
//    if (fres != FR_OK) {
//        HAL_UART_Transmit(&huart2, (uint8_t*)"Error reading SD data.\r\n", 24, HAL_MAX_DELAY);
//        return;
//    }
//
//    xbee_transmit(sd_message);
//}
