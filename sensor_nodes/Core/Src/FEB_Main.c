/*
 * FEB_Main.c
 *
 *  Created on: Feb 1, 2025
 *      Author: rahilpasha
 */

// ********************************** Includes & External **********************************
#include "FEB_Main.h"

extern TIM_HandleTypeDef htim6;
extern CAN_HandleTypeDef hcan1;

// ********************************** Variables **********************************

// ********************************** Functions **********************************

void FEB_Main_Setup(void) {


#if SEND_CAN
		// Start CAN
		HAL_CAN_Start(&hcan1);
		if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
		{
			Error_Handler();
		}
#endif

#if WSS
	WSS_Init();
#endif

#if TIRETEMP
	Tire_Temp_Init();
#endif

#if ADCS
	ADC_Init();
#endif

#if STEER
		// Initialize Steering Encoder
		Steer_ENC_I2C_Init();
		Steer_ENC_I2C_Full_Read(); // Do an initial full read
#endif

#if IMU
		// I2C_Scan();
		BNO08X_Init();
#endif

	HAL_TIM_Base_Start_IT(&htim6);  // Start 1 ms timer

}

void FEB_Main_While(void) {

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
#if WSS
		WSS_Main();
#endif

#if TIRETEMP
		Tire_Temp_Main();
#endif

#if ADCS
		ADC_Main();
#endif

#if STEER
		Steer_ENC_Main();
#endif

#if SWITCH
		Coolant_ReedSW_Main();
#endif

#if IMU
		IMU_Main();
#endif

#if GPS
		GPS_Main();
#endif
	}
}
