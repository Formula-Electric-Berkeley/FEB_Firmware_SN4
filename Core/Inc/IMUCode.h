/*
 * IMUCode.h
 *
 *  Created on: Nov 3, 2024
 *      Author: dorukhanuser
 */

#ifndef INC_IMUCODE_H_
#define INC_IMUCODE_H_

#include "stm32f4xx_hal.h" // Adjust if using another STM32 series
#include <stdint.h>

// Conversion constants
#define MILLI_G_TO_MS2 0.0098067 ///< Convert milli-gs to m/s^2
#define DEGREE_SCALE 0.01        ///< Convert raw degree values

// Struct to hold a UART-RVC packet
typedef struct {
    float yaw;       ///< Yaw in degrees
    float pitch;     ///< Pitch in degrees
    float roll;      ///< Roll in degrees
    float x_accel;   ///< X acceleration in m/s^2
    float y_accel;   ///< Y acceleration in m/s^2
    float z_accel;   ///< Z acceleration in m/s^2
} BNO08x_RVC_Data;

// Function prototypes
int BNO08x_RVC_Init(UART_HandleTypeDef *huart);
int BNO08x_RVC_Read(UART_HandleTypeDef *huart);

#endif /* INC_IMUCODE_H_ */
