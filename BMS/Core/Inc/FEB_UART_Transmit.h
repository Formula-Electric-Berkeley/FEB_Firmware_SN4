
#ifndef INC_FEB_UART_TRANSMIT_H_
#define INC_FEB_UART_TRANSMIT_H_

// **************************************** Includes ****************************************

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN.h"
#include "FEB_Const.h"

// TASKS
static FEB_Task_Header_t FEB_UART_Tasks;

// **************************************** Functions ****************************************
void FEB_UART_Transmit_Process(void);
void FEB_ADBMS_UART_Transmit(void);
void FEB_UART_Transmit_PingPong_Members(void);
void FEB_SM_UART_Transmit(void);
void FEB_UART_Transmit_CAN_Error(int);

#endif /* INC_FEB_UART_TRANSMIT_H_ */
