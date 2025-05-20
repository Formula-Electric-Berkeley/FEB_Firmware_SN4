#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_


// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include "FEB_CAN_BMS.h"

// **************************************** Functions ****************************************

void FEB_CAN_Init(void);
void FEB_CAN_Filter_Config(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);
uint8_t FEB_CAN_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

void FEB_CAN_Transmit_Test_Data(CAN_HandleTypeDef* hcan);
void FEB_CAN_Transmit_Test_Data_ExtId(CAN_HandleTypeDef* hcan);

#endif /* INC_FEB_CAN_H_ */
