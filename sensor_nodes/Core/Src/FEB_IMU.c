/*
 * FEB_IMU.c
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

// **************************************** Includes & External ****************************************

#include "FEB_IMU.h"
extern I2C_HandleTypeDef hi2c3;

// ******************************************** Functions **********************************************

#define CAN_IMU_ID 0x1FF

uint8_t IMUData[8];
int16_t accelX;
int16_t accelY;
int16_t accelZ;

/** Function to Scan I2C Bus */
void I2C_Scan(void) {
    HAL_StatusTypeDef res;

    #ifdef DEBUG_IMU_I2C_SCAN
    printf("Scanning I2C Bus...\r\n");
    #endif

    for (uint8_t addr = 1; addr < 128; addr++) {
        res = HAL_I2C_IsDeviceReady(&hi2c3, (addr << 1), 1, 10);
        if (res == HAL_OK) {
            #ifdef DEBUG_IMU_I2C_SCAN
            printf("I2C Device Found at 0x%X\r\n", addr);
            #endif
        }
    }
}

/** BNO08X Initialization */
void BNO08X_Init(void) {
    uint8_t testByte;
    HAL_StatusTypeDef status;

    #ifdef DEBUG_IMU_BNO08X_INIT
    printf("Initializing BNO08X...\r\n");
    #endif

    // Perform a test read to check if the sensor responds
    status = HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, &testByte, 1, 100);

    if (status == HAL_OK) {
        #ifdef DEBUG_IMU_BNO08X_INIT
        printf("BNO08X Detected! Test Read: 0x%X\r\n", testByte);
        #endif
    } else {
        #ifdef DEBUG_IMU_BNO08X_INIT
        printf("BNO08X Not Responding! I2C Error=%d\r\n", status);
        #endif
    }
}

/** Read a Full SHTP Packet */
int BNO08X_ReadPacket(uint8_t *pBuffer, uint16_t len) {
    uint8_t header[4];
    uint16_t packet_size;

    // Read the first 4 bytes (packet header)
    if (HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, header, 4, 100) != HAL_OK) {
        #ifdef DEBUG_IMU_READ_PACKET
        printf("I2C Read Header Failed!\r\n");
        #endif
        return 0;
    }

    // Extract packet size (Little-endian format)
    packet_size = (uint16_t)(header[0] | (header[1] << 8));
    packet_size &= ~0x8000;  // Clear "continue" bit

    #ifdef DEBUG_IMU_READ_PACKET
    printf("Received Packet Size: %d bytes\r\n", packet_size);
    #endif

    // Read the full packet
    if (packet_size > len) {
        #ifdef DEBUG_IMU_READ_PACKET
        printf("Packet too large! Size=%d\r\n", packet_size);
        #endif
        return 0;
    }

    if (HAL_I2C_Master_Receive(&hi2c3, BNO085_I2C_ADDR, pBuffer, packet_size, 100) != HAL_OK) {
        #ifdef DEBUG_IMU_READ_PACKET
        printf("I2C Read Payload Failed!\r\n");
        #endif
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

        #ifdef DEBUG_IMU_GET_RAW_DATA
        printf("Received Packet from Channel %d\r\n", channel_id);
        #endif

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
            #ifdef DEBUG_IMU_GET_RAW_DATA
            printf("Accel X: %d, Y: %d, Z: %d\r\n", accelX, accelY, accelZ);
            #endif
        }
    } else {
        #ifdef DEBUG_IMU_GET_RAW_DATA
        printf("I2C Read Failed!\r\n");
        #endif
    }
}

void Fill_CAN_Data_IMU(void) {
	IMUData[0] = (accelX >> 8) & 0xFF;
	IMUData[1] = accelX & 0xFF;
	IMUData[2] = (accelY >> 8) & 0xFF;
	IMUData[3] = accelY & 0xFF;
	IMUData[4] = (accelZ >> 8) & 0xFF;
	IMUData[5] = accelZ & 0xFF;
	IMUData[6] = 0; // Padding byte
	IMUData[7] = 0; // Padding byte
}

void IMU_Main(void) {

	BNO08X_GetRawData();
	Fill_CAN_Data_IMU();
	CAN_Transmit(CAN_ID_IMU, IMUData);
}


