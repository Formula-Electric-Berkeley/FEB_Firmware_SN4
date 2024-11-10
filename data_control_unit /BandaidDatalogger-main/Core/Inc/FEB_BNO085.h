/*
 * FEB_BNO085.h
 *
 *  Created on: May 18, 2024
 *      Author: sshashi
 */

#ifndef INC_FEB_BNO085_H_
#define INC_FEB_BNO085_H_

#include <stm32f4xx_hal.h>

void FEB_BNO085_Poll(I2C_HandleTypeDef* hi2c);

void FEB_BNO085_Setup();

#endif /* INC_FEB_BNO085_H_ */
