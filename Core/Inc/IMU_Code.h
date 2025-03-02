/*
 * IMU_Code.h
 *
 *  Created on: Feb 23, 2025
 *      Author: root
 */

#ifndef INC_IMU_CODE_H_
#define INC_IMU_CODE_H_

#include "stm32f4xx_hal.h"

// Define BNO085 I2C Address
#define BNO085_I2C_ADDR (0x4A << 1)  // Use 8-bit address format required by STM32 HAL

// Function prototypes
void I2C_Scan(void);
void BNO08X_Init(void);
int BNO08X_ReadPacket(uint8_t *pBuffer, uint16_t len);
void BNO08X_GetRawData(void);

#endif /* INC_IMU_CODE_H_ */
