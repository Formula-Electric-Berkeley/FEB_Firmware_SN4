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

//	// Start CAN
	HAL_CAN_Start(&hcan1);
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

	WSS_Init();

	// Initialize Steering Encoder
	Steer_ENC_I2C_Init();
	Steer_ENC_I2C_Full_Read(); // Do an initial full read

	Tire_Temp_Init();

	// I2C_Scan();

	BNO08X_Init();

	ADC_Init();

	HAL_TIM_Base_Start_IT(&htim6);  // Start 1 ms timer

}

void FEB_Main_While(void) {

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		WSS_Main();
		Tire_Temp_Main();
		ADC_Main();

		#ifdef IS_FRONT_NODE
			Steer_ENC_Main();
		#else
			Coolant_ReedSW_Main();
			IMU_Main();
			GPS_Main();
		#endif
	}
}
