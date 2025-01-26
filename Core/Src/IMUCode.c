#include "IMUCode.h"
#include <string.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

#include <stdio.h>
// Initialize the BNO085
int BNO08x_RVC_Init(UART_HandleTypeDef *huart) {
    // IF we need initilization
    return 0;
}

int BNO08x_RVC_Read(UART_HandleTypeDef *huart) {
    uint8_t header[2];
    uint8_t buffer[17];
    //uint8_t checksum = 0;
    BNO08x_RVC_Data data;
    char debug_buffer[256]; // Buffer for debug printing

    // Controlling the header
    if (HAL_UART_Receive(huart, header, 2, HAL_MAX_DELAY) != HAL_OK) {
        sprintf(debug_buffer, "Error: Failed to receive header.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return -1;
    }

    if (header[0] != 0xAA || header[1] != 0xAA) {
        sprintf(debug_buffer, "Error: Invalid header (0x%02X 0x%02X).\r\n", header[0], header[1]);
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return -1;
    }

    // Receiving the remaining 17 bytes
    if (HAL_UART_Receive(huart, buffer, 18, HAL_MAX_DELAY) != HAL_OK) { //17
        sprintf(debug_buffer, "Error: Failed to receive payload.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return -1;
    }

    // DEBUG Verify checksum
    /*for (uint8_t i = 0; i < 16; i++) {
        checksum += buffer[i];
    }
    if (checksum != buffer[16]) {
        sprintf(debug_buffer, "Error: Checksum mismatch.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
       return -1;
    } */

    // Raw data -> Meaningful data
    int16_t raw_data[6];
    for (uint8_t i = 0; i < 6; i++) {
        raw_data[i] = (int16_t)((buffer[1 + (i * 2) + 1] << 8) | buffer[1 + (i * 2)]);
    }

    // Conversion
    data.yaw = (float)raw_data[0] * DEGREE_SCALE;
    data.pitch = (float)raw_data[1] * DEGREE_SCALE;
    data.roll = (float)raw_data[2] * DEGREE_SCALE;
    data.x_accel = (float)raw_data[3] * MILLI_G_TO_MS2;
    data.y_accel = (float)raw_data[4] * MILLI_G_TO_MS2;
    data.z_accel = (float)raw_data[5] * MILLI_G_TO_MS2;

    // Printing the data to see
    sprintf(debug_buffer, "Yaw: %.2f, Pitch: %.2f, Roll: %.2f\r\n", data.yaw, data.pitch, data.roll);
    HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
    sprintf(debug_buffer, "Accel X: %.2f, Y: %.2f, Z: %.2f\r\n", data.x_accel, data.y_accel, data.z_accel);
    HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);

    return 0;
}
