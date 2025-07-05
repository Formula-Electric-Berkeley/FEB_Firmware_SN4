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

#include "FEB_Comms.h"

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// **************************************** Debug Macros ****************************************

// GPS debug output - comment out to disable
#define DEBUG_GPS_PRINTF

// Granular debug macros for GPS functions
#define DEBUG_GPS_READ_DATA
#define DEBUG_GPS_PARSE_MESSAGE
#define DEBUG_GPS_CONVERT_COORDINATES

// **************************************** Functions ****************************************

void Read_GPS_Data(void);
void Parse_NMEA_Message(void);
void GPS_Main(void);

#endif /* INC_FEB_GPS_H_ */
