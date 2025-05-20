/*
 * FEB_CAN_Heartbeat.h
 *
 *  Created on: Apr 13, 2025
 *      Author: samnesh
 */

#ifndef INC_FEB_CAN_HEARTBEAT_H_
#define INC_FEB_CAN_HEARTBEAT_H_

#include <string.h>
#include "FEB_CAN.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"
 
// uint8_t heartbeat[8];
void FEB_CAN_HEARTBEAT_Transmit();
void FEB_CAN_HEARTBEAT_Init();

#endif /* INC_FEB_CAN_HEARTBEAT_H_ */
 
