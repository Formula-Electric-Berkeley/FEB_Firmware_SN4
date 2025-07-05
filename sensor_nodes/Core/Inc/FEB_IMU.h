/*
 * FEB_IMU.h
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_IMU_H_
#define INC_FEB_IMU_H_

// **************************************** Includes ****************************************

#include "FEB_Comms.h"

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Debug Macros ****************************************

// IMU debug output - comment out to disable
#define DEBUG_IMU_PRINTF

// Granular debug macros for IMU functions
#define DEBUG_IMU_I2C_SCAN
#define DEBUG_IMU_BNO08X_INIT
#define DEBUG_IMU_READ_PACKET
#define DEBUG_IMU_GET_RAW_DATA

// **************************************** Variables ****************************************

// Define BNO085 I2C Address
#define BNO085_I2C_ADDR (0x4A << 1)  // Use 8-bit address format required by STM32 HAL

// **************************************** Functions ****************************************
void I2C_Scan(void);
void BNO08X_Init(void);
int BNO08X_ReadPacket(uint8_t *pBuffer, uint16_t len);
void BNO08X_GetRawData(void);
void IMU_Main(void);

#endif /* INC_FEB_IMU_H_ */
