#ifndef INC_FEB_IVT_H_
#define INC_FEB_IVT_H_

// ******************************** Includes ********************************

#include "FEB_CAN_ID.h"
#include "FEB_Const.h"
#include "FEB_SM.h"

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

// ******************************** Includes ********************************

typedef struct {
	volatile int32_t current_mA;
	volatile int32_t voltage_1_mV;
	volatile int32_t voltage_2_mV;
	volatile int32_t voltage_3_mV;
} FEB_CAN_IVT_Message_t;

// ******************************** Functions ********************************

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t rx_data[]);
void FEB_CAN_IVT_Process(void);

#endif /* INC_FEB_IVT_H_ */
