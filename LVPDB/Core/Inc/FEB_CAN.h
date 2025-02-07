#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_

// **************************************** Includes ****************************************


#include "FEB_CAN_APPS.h"
#include "FEB_CAN_ICS.h"
#include "stm32f4xx_hal.h"

typedef struct __attribute__((packed)) {
	uint32_t tim_ms; // rollover handled by logging device
	/*
	 *  Flags:
	 *  Bit 31: New current reading ready
	 *  Bit 30: New bus voltage reading ready
	 *  Bit 29: Bus undervoltage (Todo: define metric for undervoltage)
	 *  Bit 28: Bus overvoltage (Todo: define metric for overvoltage)
	 *  Bits [24:27] ID #
	 *  Bits [16:23] Bus shutdown (V_bus = 0 -> fuse blown)
	 *  Bits [8:15]: Power good
	 *  Bits [0:7]: Alert pins for overcurrent
	 */
	uint32_t flags;
	float bus_voltage; // They all run off of the same v_bus
	float lv_current;
	float cp_current;
	float af_current;
	float rf_current;
	float sh_current;
	float l_current;
	float as_current;
	float ab_current;
	uint32_t ids[9]; // 9 messages total so 9 id's sent out
} FEB_LVPDB_CAN_Data; // Make sure that all edits do not input bit stuffing

// **************************************** Functions ****************************************

void FEB_CAN_Init(void);
void FEB_CAN_Filter_Config(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);
void FEB_CAN_Transmit(CAN_HandleTypeDef *hcan, FEB_LVPDB_CAN_Data *can_data);
#endif /* INC_FEB_CAN_H_ */
