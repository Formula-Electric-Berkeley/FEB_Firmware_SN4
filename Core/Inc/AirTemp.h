/*
 * AirTemp.h
 *
 * Author: Dorukhan User
 */

#ifndef INC_AIRTEMP_H_
#define INC_AIRTEMP_H_

#include <stdint.h>

void AirTemp_Init();
float Read_Temperature();
float Convert_To_Temperature(uint32_t adc_value);

#endif /* INC_AIRTEMP_H_ */
