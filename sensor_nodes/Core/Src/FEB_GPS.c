/*
 * FEB_GPS.c
 *
 *  Created on: Feb 9, 2025
 *      Author: rahilpasha
 */

#include "FEB_GPS.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

float longitude_num;
float latitude_num;
uint8_t GPS_Buffer[164];
char *token;
char *saveptr;
char longitude[20];
char latitude[20];
int count;
void Read_GPS_Data(void)
{

	if (HAL_UART_Receive(&huart4, GPS_Buffer, sizeof(GPS_Buffer), HAL_MAX_DELAY) != HAL_OK)
	{
		char msg[50];
		sprintf(msg, "UART GPS receiving error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);
//		Error_Handler();
	}

}
float convertToDegrees(const char *coord) {
    char degreesPart[10], minutesPart[10];  // Buffers for degrees and minutes
    char *dotPos = strchr(coord, '.');  // Find the position of the period

    if (dotPos == NULL || (dotPos - coord) < 2) {
        printf("Invalid coordinate format: %s\n", coord);
        return 0.0;
    }

    int totalDigitsBeforeDot = dotPos - coord;  // Number of digits before '.'
    
    // Extract degrees (all digits except last two before '.')
    int degreeLength = totalDigitsBeforeDot - 2;
    strncpy(degreesPart, coord, degreeLength);
    degreesPart[degreeLength] = '\0';  // Null-terminate

    // Extract minutes (last two digits before '.' + digits after '.')
    strcpy(minutesPart, coord + degreeLength);

    // Convert to float values
    int degrees = atoi(degreesPart);
    float minutes = atof(minutesPart);

    // Convert to decimal degrees
    return degrees + (minutes / 60.0);
}

void Parse_NMEA_Message(void)
{
	if (HAL_UART_Transmit(&huart2, GPS_Buffer, sizeof(GPS_Buffer), HAL_MAX_DELAY) != HAL_OK)
	{
		char msg[50];
		sprintf(msg, "UART GPS transmitting error");
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof((uint8_t *) msg), HAL_MAX_DELAY);
//		Error_Handler();
	}
	token = strtok_r(GPS_Buffer, ",", &saveptr);
	count = 0;
	while (token != NULL) {
	        count++;
	        if (count == 4) { // The 5th token (after 4th comma) is the longitude
	        	strcpy(longitude, token);
	        }
		else if (count == 6) {  // Latitude is the 6th token
            		strcpy(latitude, token);
            		break;  // No need to continue parsing
        	}
        	token = strtok_r(NULL, ",", &saveptr);
    	}

	longitude_num = convertToDegrees(longitude);
	latitude_num = convertToDegrees(latitude);
}

void GPS_Main(void)
{
	Read_GPS_Data();
	Parse_NMEA_Message();
}
