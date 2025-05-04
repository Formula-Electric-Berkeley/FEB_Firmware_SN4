#ifndef INC_FEB_CAN_TPS_H_
#define INC_FEB_CAN_TPS_H_

// **************************************** Includes ****************************************

#include "FEB_CAN_ID.h"
#include "FEB_CAN_Library_SN4/feb_can.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "TPS2482.h"

// **************************************** Functions ****************************************

void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan);


#endif /* INC_FEB_CAN_ICS_H_ */
