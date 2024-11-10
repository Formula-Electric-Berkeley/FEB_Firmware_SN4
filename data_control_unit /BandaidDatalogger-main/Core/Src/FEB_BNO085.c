/*
 * FEB_BNO085.c
 *
 *  Created on: May 18, 2024
 *      Author: sshashi
 */

#include "FEB_BNO085.h"
#include "FEB_CircularBuffer.h"

extern I2C_HandleTypeDef hi2c1;
extern circBuffer FEBBuffer;

#define BNO_ADDR	0x4A

void FEB_BNO085_Poll(I2C_HandleTypeDef* hi2c) {
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(hi2c, BNO_ADDR, FEBBuffer.IMU_DATA, 1, 100);
	if(ret == HAL_OK){
		ret = HAL_I2C_Master_Receive(hi2c, BNO_ADDR, FEBBuffer.IMU_DATA, 23, 100);
	}
}

void FEB_BNO085_Setup() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
}
