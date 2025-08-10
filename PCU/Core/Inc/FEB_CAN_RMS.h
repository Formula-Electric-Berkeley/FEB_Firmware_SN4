
#ifndef INC_FEB_CAN_RMS_H_
#define INC_FEB_CAN_RMS_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "FEB_CAN_BMS.h"
#include "FEB_Normalized.h"
#include "FEB_Const.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"



// ********************************** Functions **********************************
void FEB_CAN_RMS_Setup(void);
void FEB_CAN_RMS_Process(void);
void FEB_CAN_RMS_Disable(void);
float FEB_CAN_RMS_getMaxTorque(void);
void FEB_CAN_RMS_Torque(void);
void FEB_CAN_ACC(void);
void FEB_CAN_RMS_Transmit_updateTorque(void);
void FEB_CAN_RMS_torqueTransmit(void);
void FEB_CAN_RMS_Init(void);
uint8_t FEB_CAN_RMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_RMS_Transmit_updateAcc(uint16_t acc0, uint16_t acc1);
void FEB_CAN_RMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);
void FEB_CAN_RMS_Transmit_paramSafety(void);
void FEB_CAN_RMS_Transmit_commDisable(void);
void FEB_CAN_RMS_Transmit_paramBroadcast(void);




#endif /* INC_FEB_CAN_RMS_H_ */
