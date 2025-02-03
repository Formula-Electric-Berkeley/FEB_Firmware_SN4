#ifndef INC_FEB_SM_H_
#define INC_FEB_SM_H_

// ******************************** Includes ********************************
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "FEB_HW.h"
#include "FEB_CAN.h"
#include "FEB_Const.h"
#include "FEB_Task_Queue.h"

// States
typedef enum {
	FEB_SM_ST_BOOT,
	FEB_SM_ST_LV,
	FEB_SM_ST_ESC,
	FEB_SM_ST_PRECHARGE,
	FEB_SM_ST_ENERGIZED,
	FEB_SM_ST_DRIVE,
	FEB_SM_ST_FREE,
	FEB_SM_ST_CHARGING,
	FEB_SM_ST_BALANCE,
	FEB_SM_ST_FAULT_BMS,
	FEB_SM_ST_FAULT_BSPD,
	FEB_SM_ST_FAULT_IMD,
	FEB_SM_ST_FAULT_CHARGING,
	FEB_SM_ST_DEFAULT
} FEB_SM_ST_t;

void FEB_SM_Init(void);
FEB_SM_ST_t FEB_SM_Get_Current_State(void);
void FEB_SM_Transition(FEB_SM_ST_t next_state);
void FEB_SM_Process(void);
void FEB_SM_UART_Transmit(void);
void FEB_SM_CAN_Transmit(void);

/* Faults
CAN Initialization fails
Under/Over cell voltage
Under/Over cell temperature
Under/Over pack current
*/

#endif /* INC_FEB_SM_H_ */
