/*
 * FEB_IMU.c
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_IMU.h"
extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c3;
extern UART_HandleTypeDef huart2;

// ******************************************** Functions **********************************************

#define CAN_IMU_ID 0x1FF
char debug_buffer[100];

uint8_t TxData[8];
int16_t accelX;
int16_t accelY;
int16_t accelZ;

/** Function to Scan I2C Bus */
void I2C_Scan(void) {
    char msg[64];
    HAL_StatusTypeDef res;

    sprintf(msg, "Scanning I2C Bus...\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    for (uint8_t addr = 1; addr < 128; addr++) {
        res = HAL_I2C_IsDeviceReady(&hi2c3, (addr << 1), 1, 10);
        if (res == HAL_OK) {
            sprintf(msg, "I2C Device Found at 0x%X\r\n", addr);
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}

/** BNO08X Initialization */
void BNO08X_Init(void) {
    uint8_t testByte;
    HAL_StatusTypeDef status;

    sprintf(debug_buffer, "Initializing BNO08X...\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);

    // Perform a test read to check if the sensor responds
    status = HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, &testByte, 1, 100);

    if (status == HAL_OK) {
        sprintf(debug_buffer, "BNO08X Detected! Test Read: 0x%X\r\n", testByte);
    } else {
        sprintf(debug_buffer, "BNO08X Not Responding! I2C Error=%d\r\n", status);
    }

    HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
}

/** Read a Full SHTP Packet */
int BNO08X_ReadPacket(uint8_t *pBuffer, uint16_t len) {
    uint8_t header[4];
    uint16_t packet_size;

    // Read the first 4 bytes (packet header)
    if (HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, header, 4, 100) != HAL_OK) {
        sprintf(debug_buffer, "I2C Read Header Failed!\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return 0;
    }

    // Extract packet size (Little-endian format)
    packet_size = (uint16_t)(header[0] | (header[1] << 8));
    packet_size &= ~0x8000;  // Clear "continue" bit

    sprintf(debug_buffer, "Received Packet Size: %d bytes\r\n", packet_size);
    HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);

    // Read the full packet
    if (packet_size > len) {
        sprintf(debug_buffer, "Packet too large! Size=%d\r\n", packet_size);
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return 0;
    }

    if (HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, pBuffer, packet_size, 100) != HAL_OK) {
        sprintf(debug_buffer, "I2C Read Payload Failed!\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        return 0;
    }

    return packet_size;
}

/** Read Raw Data from BNO08X */
void BNO08X_GetRawData(void) {
    uint8_t rawData[276];  // Large enough for any packet
    int packetSize = BNO08X_ReadPacket(rawData, sizeof(rawData));

    if (packetSize > 0) {
        // Extract channel ID
        uint8_t channel_id = rawData[2];

        sprintf(debug_buffer, "Received Packet from Channel %d\r\n", channel_id);
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);

        // If this is sensor data, process it
        if (channel_id == 2) {  // Change channel ID if needed
            accelX = (int16_t)((rawData[5] << 8) | rawData[4]);
            accelY = (int16_t)((rawData[7] << 8) | rawData[6]);
            accelZ = (int16_t)((rawData[9] << 8) | rawData[8]);

//            float ax = accelX / 1024.0f;
//            float ay = accelY / 1024.0f;
//            float az = accelZ / 1024.0f;
//
//            sprintf(debug_buffer, "Accel X: %.2f, Y: %.2f, Z: %.2f\r\n", ax, ay, az);
            sprintf(debug_buffer, "Accel X: %d, Y: %d, Z: %d\r\n", accelX, accelY, accelZ);
            HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
        }
    } else {
        sprintf(debug_buffer, "I2C Read Failed!\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
    }
}

void Fill_CAN_Data(void) {
    TxData[0] = (accelX >> 8) & 0xFF;
    TxData[1] = accelX & 0xFF;
    TxData[2] = (accelY >> 8) & 0xFF;
    TxData[3] = accelY & 0xFF;
    TxData[4] = (accelZ >> 8) & 0xFF;
    TxData[5] = accelZ & 0xFF;
    TxData[6] = 0; // Padding byte
    TxData[7] = 0; // Padding byte
}

void CAN_IMU_Transmit(void) {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;

    TxHeader.DLC = 8; // Data length
    TxHeader.IDE = CAN_ID_STD; // Standard ID
    TxHeader.RTR = CAN_RTR_DATA; // Data frame
    TxHeader.StdId = CAN_IMU_ID; // Example CAN ID for IMU data
    TxHeader.ExtId = 0; // Not used with standard ID

    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {} // Wait for a free mailbox

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        // Transmission request error
    }
}



void IMU_Main(void) {

	BNO08X_GetRawData();
	Fill_CAN_Data();
	CAN_IMU_Transmit();

}
