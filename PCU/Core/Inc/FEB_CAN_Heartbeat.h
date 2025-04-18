/*
 * FEB_CAN_Heartbeat.h
 *
 *  Created on: Apr 13, 2025
 *      Author: samnesh
 */

 #ifndef INC_FEB_CAN_HEARTBEAT_H_
 #define INC_FEB_CAN_HEARTBEAT_H_
 
 #include "FEB_CAN_BMS.h"
 #include <string.h>
 #include "FEB_CAN_Library_SN4/gen/feb_can.h"
 
 void FEB_CAN_HEARTBEAT_Transmit(CAN_HandleTypeDef *hcan, uint64_t *bms_errors);
 
 #endif /* INC_FEB_CAN_HEARTBEAT_H_ */
 