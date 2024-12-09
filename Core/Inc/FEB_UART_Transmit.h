#ifndef INC_FEB_UART_TRANSMIT_H_
#define INC_FEB_UART_TRANSMIT_H_

// **************************************** Includes ****************************************

#include <stdint.h>

#include <string.h>

#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "FEB_CAN_ICS.h"

// **************************************** Functions ****************************************
void FEB_UART_Transmit_Process(int);

void FEB_UART_Transmit_Speed(void);

#endif /* INC_FEB_UART_TRANSMIT_H_ */
