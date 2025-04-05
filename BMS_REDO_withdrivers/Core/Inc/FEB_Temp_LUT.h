#ifndef INC_FEB_TEMP_LUT_H_
#define INC_FEB_TEMP_LUT_H_

// **************** Includes ****************

#include <stdint.h>

// **************** Macros ****************

#define FEB_TEMP_LUT_MIN_VOLT_mV 1300
#define FEB_TEMP_LUT_MAX_VOLT_mV 2380

// **************** Functions ****************

int16_t FEB_Temp_LUT_Get_Temp_100mC(uint16_t voltage_mV);

#endif /* INC_FEB_TEMP_LUT_H_ */
