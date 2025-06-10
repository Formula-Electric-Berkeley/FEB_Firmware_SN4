#ifndef INC_FEB_IVT_H_
#define INC_FEB_IVT_H_

//IVT controls (if we are still using IVT)
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN_IDs.h"
#include "FEB_Const.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_SM.h"


typedef struct {
	volatile int32_t current_mA;
	volatile int32_t voltage_1_mV;
	volatile int32_t voltage_2_mV;
	volatile int32_t voltage_3_mV;
} FEB_CAN_IVT_Message_t;

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t rx_data[]);
void FEB_CAN_IVT_Process(void);
int32_t FEB_IVT_V1_Voltage();
int32_t FEB_IVT_V1_Current();
void FEB_IVT_Serial(void);
#endif /* INC_FEB_IVT_H_ */



