/*
 * FEB_Main.h
 *
 *  Created on: Jan 23, 2025
 *      Author: root
 */

#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "IMUCode.h"
#include <stdio.h>
#include <string.h>

void FEB_Main_Setup(void);

void FEB_Main_Loop(void);

#endif /* INC_FEB_MAIN_H_ */
