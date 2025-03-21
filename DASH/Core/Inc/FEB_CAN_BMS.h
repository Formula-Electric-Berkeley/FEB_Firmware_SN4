#ifndef INC_FEB_CAN_BMS_H_
#define INC_FEB_CAN_BMS_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include "FEB_CAN_Library/FEB_CAN_ID.h"
#include <stdio.h>
#include <stdbool.h>
#include <FEB_IO_ICS.h>


// ********************************** Functions **********************************


typedef enum {
	FEB_SM_ST_STARTUP,
	FEB_SM_ST_STANDBY,
	FEB_SM_ST_BALANCE,
	FEB_SM_ST_CHARGE,
	FEB_SM_ST_PRECHARGE,
	FEB_SM_ST_DRIVE_STANDBY,
	FEB_SM_ST_DRIVE,
	FEB_SM_ST_DRIVE_REGEN,
	FEB_SM_ST_FAULT
} FEB_SM_ST_t;


uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);
FEB_SM_ST_t FEB_CAN_BMS_Get_State(); // Check if still in drivable state
bool FEB_CAN_BMS_is_stale(); //Check if message is stale



#endif /* INC_FEB_CAN_BMS_H_ */
