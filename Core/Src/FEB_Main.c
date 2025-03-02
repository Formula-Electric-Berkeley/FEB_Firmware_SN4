/*
 * FEB_Main.c
 *
 *  Created on: Feb 23, 2025
 *      Author: root
 */
#include "FEB_Main.h"
#include "IMU_Code.h"
#include <stdio.h>
#include <string.h>


void FEB_Setup(void) {
    I2C_Scan();  // Scan for I2C devices
    HAL_Delay(500);

    BNO08X_Init();  // Initialize sensor
    HAL_Delay(500);
}

void FEB_Loop(void) {
    BNO08X_GetRawData();  // Read raw data
    HAL_Delay(500);
}


