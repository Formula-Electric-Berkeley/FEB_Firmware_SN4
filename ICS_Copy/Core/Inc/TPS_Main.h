/*
 * TPS_Main.h
 *
 *  Created on: Apr 29, 2025
 *      Author: root
 */

#ifndef INC_TPS_MAIN_H_
#define INC_TPS_MAIN_H_

#include <TPS2482.h>

#define LV_ADDR TPS2482_I2C_ADDR(TPS2482_I2C_ADDR_GND,TPS2482_I2C_ADDR_GND) // A1:GND 	A0:GND
#define R_SHUNT	(double)(.002)
//#define LV_CURRENT_LSB	TPS2482_CURRENT_LSB_EQ(LV_FUSE_MAX)

#define NUM_TPS2482	8

#define FLOAT_TO_UINT16_T(n)		((uint16_t)(n * 1000)) // for voltage (mV)
#define FLOAT_TO_INT16_T(n)			((int16_t)(n * 1000)) // for voltage (mV)
#define SIGN_MAGNITUDE(n)			(int16_t)((((n >> 15) & 0x01) == 1) ? -(n & 0x7FFF) : (n & 0x7FFF)) // for current reg

#define LV_CAL_VAL	TPS2482_CAL_EQ(LV_CURRENT_LSB,R_SHUNT)

#define LV_CAL_CORRECTED 	5838

#define LV_ALERT_LIM_VAL	TPS2482_SHUNT_VOLT_REG_VAL_EQ((uint16_t)(LV_FUSE_MAX / LV_CURRENT_LSB),LV_CAL_VAL)

#define LV_POWER_LSB	TPS2482_POWER_LSB_EQ(LV_CURRENT_LSB)

#define FEB_BREAK_THRESHOLD	(uint8_t)10
#define SLEEP_TIME 10
void FEB_Variable_Init(void);
void FEB_Variable_Conversion(void);

#endif /* INC_TPS_MAIN_H_ */
