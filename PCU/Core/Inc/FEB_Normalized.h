#ifndef INC_FEB_NORMALIZED_H_
#define INC_FEB_NORMALIZED_H_

// **************************************** Includes ****************************************


#include "FEB_Const.h"

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FEB_CAN_Library_SN4/gen/feb_can.h"

// **************************************** Functions ****************************************

void FEB_Read_Accel_Pedal1();
void FEB_Read_Accel_Pedal2();
void FEB_Read_Brake_Pedal();
uint16_t FEB_Read_ADC(uint32_t channel);

float FEB_Normalized_getAcc();
void FEB_Normalized_setAcc0();
void FEB_Normalized_updateAcc();
float FEB_Normalized_Acc_Pedals();
float FEB_Normalized_getBrake();
void FEB_Normalized_update_Brake();
float FEB_Normalized_Brake_Pedals();
uint16_t FEB_Calculate_PSI1();
uint16_t FEB_Calculate_PSI2();
void FEB_Normalized_CAN_sendBrake();
void FEB_Normalized_Test_Acc();
void FEB_Soft_BSPD();

#endif /* INC_FEB_NORMALIZED_H_ */
