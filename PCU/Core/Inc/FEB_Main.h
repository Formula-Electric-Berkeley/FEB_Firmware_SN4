#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

// ******************************** Includes ********************************

#include <FEB_CAN.h>
#include <stm32f4xx_hal.h>
#include <stdint.h>

#include "FEB_Timer.h"
#include "FEB_CAN_RMS.h"
#include "FEB_CAN_ICS.h"
#include "FEB_CAN_BSPD.h"
#include "FEB_CAN_BMS.h"
#include "FEB_Normalized.h"
#include "FEB_TPS2482.h"
#include "FEB_HECS.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"
#include "FEB_CAN_Heartbeat.h"


// ******************************** Functions ********************************

void FEB_Main_Setup(void);
void FEB_Main_While(void);

#endif /* INC_FEB_MAIN_H_ */
