#ifndef INC_FEB_CAN_ICS_H_
#define INC_FEB_CAN_ICS_H_

// **************************************** Includes ****************************************

#include "FEB_CAN_ID.h"

#include "stm32f4xx_hal.h"

// **************************************** Functions ****************************************

uint8_t FEB_CAN_ICS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

void FEB_CAN_ICS_Rx_Handler(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]);

int FEB_CAN_ICS_Transmit(void);

uint8_t FEB_CAN_ICS_Get_Speed(void);

#endif /* INC_FEB_CAN_ICS_H_ */
