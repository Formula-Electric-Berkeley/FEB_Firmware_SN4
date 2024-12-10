/*
 * FEB_Main.c
 *
 *  Created on: Dec 9, 2024
 *      Author: rahilpasha
 */
#include "FEB_Main.h"
#include "FEB_GPS.h"

void FEB_Main_Setup(void)
{

}

void FEB_Main_Loop(void)
{
	Read_GPS_Data();
	Parse_NMEA_Message();
	HAL_Delay(500);
}
