#ifndef INC_FEB_DASH_CAN_H_
#define INC_FEB_DASH_CAN_H_

//IVT controls (if we are still using IVT)
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN_IDs.h"
#include "FEB_Const.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_SM.h"


typedef struct {
	bool r2r;
} FEB_CAN_DASH_Message_t;

uint8_t FEB_CAN_DASH_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_DASH_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t rx_data[]);
bool FEB_CAN_DASH_Get_R2R();

#endif /* INC_FEB_DASH_CAN_H_ */



