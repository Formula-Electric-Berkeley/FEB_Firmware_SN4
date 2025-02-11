/*
 * CoolantSensor.h
 *
 *  Created on: Feb 9, 2025
 *      Author: root
 */

#ifndef INC_COOLANTSENSOR_H_
#define INC_COOLANTSENSOR_H_

#include "stm32f4xx_hal.h"
#include <math.h>

#define SERIES_RESISTOR 10000 // 10K Ohm
#define BETA_COEFFICIENT 3950 // Beta value for thermistor
#define NOMINAL_TEMPERATURE 298.15 // 25°C in Kelvin
#define NOMINAL_RESISTANCE 10000.0 // 10K Ohm at 25°C

extern volatile uint32_t pulse_count;

float getTemperature(void);
float getFlowRate(void);


#endif /* INC_COOLANTSENSOR_H_ */
