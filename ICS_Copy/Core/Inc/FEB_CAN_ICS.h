#ifndef INC_FEB_CAN_ICS_H_
#define INC_FEB_CAN_ICS_H_

// **************************************** Includes ****************************************

#include <FEB_CAN_Library_SN4/gen/feb_can.h>
#include "FEB_UART_TRANSMIT.h"
#include "FEB_IO_ICS.h"
#include "FEB_CAN_APPS.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include "lvgl.h"
#include "ui.h"

typedef struct {
	volatile uint32_t id;
	volatile uint32_t dlc;
	volatile uint8_t data[8];
} ICS_CAN_Rx_t;

typedef struct {
	volatile uint8_t speed;
} FEB_CAN_ICS_Message_t;

typedef struct {
	volatile uint8_t bms_state;
	volatile float ivt_voltage;
	volatile uint16_t acc_temp;
	volatile uint16_t min_voltage;
	volatile uint16_t pack_voltage;
	volatile uint16_t motor_speed;
} ICS_UI_Values_t;

// **************************************** Functions ****************************************

uint8_t FEB_CAN_ICS_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

void FEB_CAN_ICS_Rx_Handler(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]);

void FEB_CAN_ICS_Transmit_Button_State(uint8_t transmit_button_state);

void FEB_CAN_ICS_Transmit_Speed(void);

uint8_t FEB_CAN_ICS_Get_Speed(void);

#endif /* INC_FEB_CAN_ICS_H_ */
