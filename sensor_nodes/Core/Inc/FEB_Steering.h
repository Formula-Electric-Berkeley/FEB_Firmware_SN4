/*
 * FEB_Steering.h
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_STEERING_H_
#define INC_FEB_STEERING_H_

// **************************************** Includes ****************************************

#include "FEB_Comms.h"

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// **************************************** Functions ****************************************

bool Steer_ENC_I2C_Init(void);
void Steer_ENC_I2C_Read(void);
void Steer_ENC_I2C_Full_Read(void);
void Steer_ENC_CAN_Message(uint8_t *canTx);
void Steer_ENC_Main(void);

#endif /* INC_FEB_STEERING_H_ */
