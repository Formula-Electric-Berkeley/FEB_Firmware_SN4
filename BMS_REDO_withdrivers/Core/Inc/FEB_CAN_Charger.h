#ifndef INC_FEB_CHARGE_H_
#define INC_FEB_CHARGE_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"

void FEB_CAN_Charger_Init(void);
uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t filter_bank);
void FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);
bool FEB_CAN_Charger_Received();
void FEB_CAN_Charger_Process(void);
void FEB_CAN_Charger_UART_Transmit(void);

#endif /* INC_FEB_CHARGE_H_ */
