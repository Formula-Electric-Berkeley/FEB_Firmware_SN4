/*
 * FEB_IMU.h
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_IMU_H_
#define INC_FEB_IMU_H_

// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// **************************************** Variables ****************************************

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

// **************************************** Functions ****************************************
int BNO08x_RVC_Init();
int BNO08x_RVC_Read();


#endif /* INC_FEB_IMU_H_ */
