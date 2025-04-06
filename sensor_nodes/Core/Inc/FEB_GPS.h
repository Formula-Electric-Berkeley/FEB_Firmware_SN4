/*
 * FEB_GPS.h
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_GPS_H_
#define INC_FEB_GPS_H_

#include "FEB_Main.h"

// **************************************** Includes ****************************************

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// **************************************** Functions ****************************************

void Read_GPS_Data(void);
void Parse_NMEA_Message(void);
void GPS_Main(void);

#endif /* INC_FEB_GPS_H_ */
