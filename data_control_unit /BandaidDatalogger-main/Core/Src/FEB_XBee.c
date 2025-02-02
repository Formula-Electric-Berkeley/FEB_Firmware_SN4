/*
 * FEB_XBee.c
 *
 *  Created on: Feb 1, 2025
 *      Author: mihirtakalkar
 */

#include "FEB_XBee.h"

extern UART_HandleTypeDef huart1;  // TODO: Edit based on IOC

void xbee_transmit(const char *message) {
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

void xbee_loop() {
    uint32_t counter = 0;
    char message[50];

    while (1) {
        snprintf(message, sizeof(message), "XBee TX: %lu\r\n", counter);
        xbee_transmit(message);

        counter++;  // Increment counter
        HAL_Delay(1000);  // Delay 1 second before next transmission
    }
}

// Transmit CAN data in circbuffer
void xbee_transmit_can_data(circBuffer *cb) {
    if (cb->count == 0) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error! No CAN data to send.\r\n", 29, HAL_MAX_DELAY);
        return;
    }

    char can_message[128];
    int len = snprintf(can_message, sizeof(can_message), "CAN TX: Time(ms): %lu, ID: %u, Data:",
                       cb->buffer[cb->read].timestamp, cb->buffer[cb->read].id);

    for (int i = 0; i < 8; i++) {
        len += snprintf(can_message + len, sizeof(can_message) - len, " %02X", cb->buffer[cb->read].data[i]);
    }
    strcat(can_message, "\r\n");

    xbee_transmit(can_message);

    // Move to next entry in circ buf
    cb->read = (cb->read + 1) % cb->capacity;
    cb->count--;
}

// Alternatively, transmit sd card data
void xbee_transmit_sd_data() {
    char sd_message[128];

    fres = f_lseek(&fil, f_size(&fil) - sizeof(sd_message));  // last entry
    if (fres != FR_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error finding last SD entry.\r\n", 30, HAL_MAX_DELAY);
        return;
    }

    fres = f_read(&fil, sd_message, sizeof(sd_message), &br);
    if (fres != FR_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error reading SD data.\r\n", 24, HAL_MAX_DELAY);
        return;
    }

    xbee_transmit(sd_message);
}
