#ifndef INC_FEB_CAN_APPS_H_
#define INC_FEB_CAN_APPS_H_

#include <FEB_CAN_Library_SN4/gen/feb_can.h>
#include <stdint.h>
#include <string.h>


#include "stm32f4xx_hal.h"



typedef struct {
	uint8_t brake_pedal; // Should be some value between 0 and 100
	float current;
	uint8_t enabled;
} FEB_CAN_APPS_Message_t;

uint8_t FEB_CAN_APPS_Get_Enabled();
void FEB_CAN_APPS_Str_Msg(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]);
uint8_t FEB_CAN_APPS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
uint8_t FEB_CAN_APPS_Get_Brake_Pos();



#endif /* INC_FEB_CAN_APPS_H_ */
