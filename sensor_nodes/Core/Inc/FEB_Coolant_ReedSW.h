/*
 * FEB_Coolant_ReedSW.h
 *
 *  Created on: Apr 12, 2025
 *      Author: rahilpasha
 */

#ifndef INC_FEB_COOLANT_REEDSW_H_
#define INC_FEB_COOLANT_REEDSW_H_

// **************************************** Includes ****************************************

#include "FEB_Comms.h"
#include "main.h"

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// **************************************** Debug Macros ****************************************

// Coolant Reed Switch debug output - comment out to disable
#define DEBUG_COOLANT_REEDSW_PRINTF

// Granular debug macros for Coolant Reed Switch functions
#define DEBUG_COOLANT_REEDSW_MAIN

// **************************************** Functions ****************************************

void Coolant_ReedSW_Main(void);

#endif /* INC_FEB_COOLANT_REEDSW_H_ */
