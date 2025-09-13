// **************************************** Includes & External ****************************************

#include "FEB_Normalized.h"
#include <math.h>

extern CAN_HandleTypeDef hcan1;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;

extern UART_HandleTypeDef huart2;



const uint16_t Sensor_Min = 4095.0/5.0*0.5;
const uint16_t Sensor_Max = 4095.0/5.0*4.5;

// **************************************** Configuration ****************************************


float normalized_acc;
float normalized_brake;
uint16_t psi1; 
uint16_t psi2; 

bool isImpl = false;
bool acc_valid = false;  // Track if acceleration has been properly updated
//float time = 0.0;

//float normalizedTrigger = 0.1; // portion of brake pedal we can press before we trigger a the soft BSPD
//float timeThreshold = 1000; // amount of cycles that needs to elapse before trigger, 12.5 nanoseconds / cycle

// **************************************** Functions ****************************************

uint16_t FEB_Read_ADC(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig={0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
//	   Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	return (uint16_t)HAL_ADC_GetValue(&hadc1);

}

float FEB_Normalized_getAcc(){
	return normalized_acc;
}

void FEB_Normalized_setAcc0(){
	normalized_acc = 0.0;
	acc_valid = false;  // Mark as invalid when forced to 0
	
	// Debug logging
	char buf[128];
	int buf_len;
	buf_len = snprintf(buf, sizeof(buf), "[ACC_SET_0] Acc forced to 0, valid=false\r\n");
	if (buf_len > 0 && buf_len < sizeof(buf)) {
		HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
	}
}

//// unused (hopefully)
//void FEB_Read_Accel_Pedal1() {
//	uint16_t accel_pedal_1_raw = FEB_Read_ADC(ACC_PEDAL_1);
//
//	float accel_pedal_1_position = 0.03256 * accel_pedal_1_raw - 13.4;
//
//
//	if (accel_pedal_1_position > 100.0) {
//		accel_pedal_1_position = 100.0;
//	}
//
//	if (accel_pedal_1_position < 0.0) {
//		accel_pedal_1_position = 0.0;
//	}
//
//	//int accel_pedal_1_position_int1 = accel_pedal_1_position; //Todo
//	//float accel_pedal_1_position_frac = accel_pedal_1_position - accel_pedal_1_position_int1;
//	//int accel_pedal_1_position_int2 = accel_pedal_1_position_frac * 1000;
//
//	char buf[128];
//	sprintf(buf, "[SENSOR] Accelerator 1 Position RAW: %d\n\r", accel_pedal_1_raw);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
//
//	char buf1[128];
//	sprintf(buf1, "[SENSOR] Accelerator 1 Position: %f\n\r", accel_pedal_1_position);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
//}
//
//
//// unused (hopefully)
//void FEB_Read_Accel_Pedal2() {
//	uint16_t accel_pedal_2_raw = FEB_Read_ADC(ACC_PEDAL_2);
//
//	float accel_pedal_2_position = 0.36437 * accel_pedal_2_raw - 114.8547;
//
//	if (accel_pedal_2_position > 100.0) {
//		accel_pedal_2_position = 100.0;
//	}
//
//	if (accel_pedal_2_position < 0.0) {
//		accel_pedal_2_position = 0.0;
//	}
//
//	int accel_pedal_2_position_int1 = accel_pedal_2_position;
//	float accel_pedal_2_position_frac = accel_pedal_2_position - accel_pedal_2_position_int1;
//	int accel_pedal_2_position_int2 = accel_pedal_2_position_frac * 1000;
//
//	char buf[128];
//	sprintf(buf, "[SENSOR] Accelerator 2 Position RAW: %d\n\r", accel_pedal_2_raw);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
//
//	char buf1[128];
//	sprintf(buf1, "[SENSOR] Accelerator 2 Position: %d.%d\n\r", accel_pedal_2_position_int1, accel_pedal_2_position_int2);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
//}

void FEB_Read_Brake_Pedal() {
	uint16_t brake_pedal_raw = FEB_Read_ADC(BRAKE_IN);

	float brake_pedal_position = 0.03256 * brake_pedal_raw - 13.4;

	if (brake_pedal_position > 100.0) {
		brake_pedal_position = 100.0;
	}

	if (brake_pedal_position < 0.0) {
		brake_pedal_position = 0.0;
	}

	int brake_pedal_position_int1 = brake_pedal_position;
	float brake_pedal_position_frac = brake_pedal_position - brake_pedal_position_int1;
	int brake_pedal_position_int2 = brake_pedal_position_frac * 1000;

	char buf[128];
	sprintf(buf, "[SENSOR] Brake Position RAW: %d\r\n", brake_pedal_raw);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	char buf1[128];
	sprintf(buf1, "[SENSOR] Brake Position: %d.%d\r\n", brake_pedal_position_int1, brake_pedal_position_int2);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
}

void FEB_Normalized_updateAcc(){
	// Get the new acceleration value
	float new_acc = FEB_Normalized_Acc_Pedals();
	
	// Atomically update both the value and validity flag
	// This prevents race conditions where torque could be commanded with stale data
	__disable_irq();  // Critical section start
	normalized_acc = new_acc;
	acc_valid = true;  // Mark as valid after successful update
	__enable_irq();   // Critical section end

	// Debug logging
	char buf[128];
	int buf_len;
	buf_len = snprintf(buf, sizeof(buf), "[ACC_UPDATE] normalized_acc: %.3f, valid=true\r\n", normalized_acc);
	if (buf_len > 0 && buf_len < sizeof(buf)) {
		HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, HAL_MAX_DELAY);
	}
}

bool FEB_Normalized_isAccValid(){
	return acc_valid;
}

float FEB_Normalized_Acc_Pedals() {
	// raw ADC readings of the two acc pedal sensors
	uint16_t acc_pedal_1 = FEB_Read_ADC(ACC_PEDAL_1);
	uint16_t acc_pedal_2 = FEB_Read_ADC(ACC_PEDAL_2);

	//convert to % travel
	// sensor 1 has positive slope
	float ped1_normalized = (acc_pedal_1 - ACC_PEDAL_1_START)/ (1.0f*ACC_PEDAL_1_END - ACC_PEDAL_1_START);
	// sensor 2 has negative slope
	float ped2_normalized = (acc_pedal_2 - ACC_PEDAL_2_START) / (1.0f*ACC_PEDAL_2_END - ACC_PEDAL_2_START);
	
	char msg[100];
	snprintf(msg, sizeof(msg), "Pedal1: %.3f | Pedal2: %.3f | isImpl: %d\n\r", ped1_normalized, ped2_normalized, isImpl);
	HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	
	#if TORQUE_TEST_MODE
	// In test mode, bypass all implausibility checks and just return the average
	float final_normalized = 0.5f * (ped1_normalized + ped2_normalized);
	final_normalized = (final_normalized < 0.0f) ? 0.0f : (final_normalized > 1.0f ? 1.0f : final_normalized);
	
	// Clear implausibility flag in test mode
	if (isImpl) {
		char impl_msg[128];
		snprintf(impl_msg, sizeof(impl_msg), "[TEST_MODE] Bypassing implausibility, returning %.3f\n\r", final_normalized);
		HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
		isImpl = false;
	}
	
	return final_normalized;
	#endif

	// check implausibility for shorting. TODO: check if this fulfills the short circuiting rules.
	if (acc_pedal_1 > ACC_PEDAL_1_END + 20 || acc_pedal_1 < ACC_PEDAL_1_START - 20
			|| acc_pedal_2 < ACC_PEDAL_2_START - 20 || acc_pedal_2 > ACC_PEDAL_2_END + 20) {
		if (!isImpl) {
			char impl_msg[128];
			snprintf(impl_msg, sizeof(impl_msg), "[IMPL_SET] Out of range: P1=%d P2=%d\r\n", acc_pedal_1, acc_pedal_2);
			HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
		}
		isImpl = true;
		return 0.0;
	}

	// print out values



	// sensor measurements mismatch by more than 10%
	if(fabs(ped1_normalized - ped2_normalized) > 0.1 ){
		if (!isImpl) {
			char impl_msg[128];
			snprintf(impl_msg, sizeof(impl_msg), "[IMPL_SET] Mismatch: P1=%.3f P2=%.3f diff=%.3f\r\n", 
					ped1_normalized, ped2_normalized, fabs(ped1_normalized - ped2_normalized));
			HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
		}
		isImpl = true;
		return 0.0;
	}

	// PA7 pin
//	GPIO_PinState bspd_reading = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
//	unsigned char buf[128];
//	uint8_t buf_len;
//	buf_len = sprintf(buf, "pin state:%u\n\r", (uint8_t) bspd_reading);
//	HAL_UART_Transmit(&huart2, buf, buf_len, HAL_MAX_DELAY);

	// if ((uint8_t) bspd_reading == 1) { // correct is low
	// 	normalized_acc = 0.0;
	// }

	// // Implausiblity check if both pedals are stepped
	// if (normalized_brake > 0.2 && normalized_acc > 0.1) {
	// 	isImpl = true;
	// }

	// float final_normalized = 0.5 * (ped1_normalized + ped2_normalized);
	// final_normalized = final_normalized<0?0:(final_normalized>1?1:final_normalized);

	// // recover from implausibility if acc pedal is not 5% less
	// if (final_normalized < 0.05 && isImpl) {
	// 	isImpl = false;
	// }

	// if (!isImpl) {
	// 	return final_normalized;
	// } else {
	// 	return 0.0;
	// }

	// Calculate final normalized accelerator value
	float final_normalized_val = 0.5f * (ped1_normalized + ped2_normalized); // take the average of the two sensor inputs
	final_normalized_val = (final_normalized_val < 0.0f) ? 0.0f : (final_normalized_val > 1.0f ? 1.0f : final_normalized_val); //clamp between 0 and 1

	// BSPD digital read protection (active-low)
	// GPIO_PinState bspd_reading = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	// if ((uint8_t)bspd_reading == 1) {
	// 	final_normalized = 0.0f;
	// }

	// Brake + accelerator conflict → disable accelerator
	//	if (normalized_brake > 0.2f && final_normalized > 0.05f) {
	//		isImpl = true;
	//	}

	// Only recover once both pedals are fully released
	// In test mode, only require accelerator to be released for recovery
	#if TORQUE_TEST_MODE
	if (final_normalized_val < 0.05f && isImpl) {
		char impl_msg[128];
		snprintf(impl_msg, sizeof(impl_msg), "[IMPL_CLEAR] Test mode recovery: Acc=%.3f\r\n", final_normalized_val);
		HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
		isImpl = false;
	}
	#else
	if (final_normalized_val < 0.05f && normalized_brake < 0.15f && isImpl) {
		char impl_msg[128];
		snprintf(impl_msg, sizeof(impl_msg), "[IMPL_CLEAR] Normal recovery: Acc=%.3f Brk=%.3f\r\n", 
				final_normalized_val, normalized_brake);
		HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
		isImpl = false;
	}
	#endif

	// Apply implausibility override
	if (!isImpl) {
		return final_normalized_val;
	} else {
		// Log when returning 0 due to implausibility
		static uint32_t last_impl_log = 0;
		uint32_t current_time = HAL_GetTick();
		if (current_time - last_impl_log > 1000) {  // Log once per second max
			char impl_msg[128];
			snprintf(impl_msg, sizeof(impl_msg), "[IMPL_ACTIVE] Returning 0, need release\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)impl_msg, strlen(impl_msg), HAL_MAX_DELAY);
			last_impl_log = current_time;
		}
		return 0.0f;
	}
}

float FEB_Normalized_getBrake() {
	return normalized_brake;
}

void FEB_Normalized_update_Brake() {
	normalized_brake = FEB_Normalized_Brake_Pedals();
	psi1 = FEB_Calculate_PSI1();
	psi2 = FEB_Calculate_PSI2();

}

float FEB_Normalized_Brake_Pedals() {
	//TODO: This might need to change based on which sensor ends up getting used.
	uint16_t brake_pres =  FEB_Read_ADC(BRAKE_IN);   //HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);

//	HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);

	float final_normalized = (brake_pres - BRAKE_PRESSURE_START)/ (BRAKE_PRESSURE_END - BRAKE_PRESSURE_START);
	// final_normalized = final_normalized > 1 ? 1 : final_normalized;
	// final_normalized = final_normalized < 0.05 ? 0 : final_normalized;

	// Debug logging
	char buf[128];
	int buf_len;
	buf_len = snprintf(buf, sizeof(buf), "brake_Pos: %.3f\r\n", final_normalized);
	if (buf_len > 0 && buf_len < sizeof(buf)) {
		// HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);  // Commented out as it was originally
	}

	// Soft BSPD
//	char buf1[1];
//	uint8_t buf_len1;
//	buf_len1 = sprintf(buf1, "time, bspd_flag:%f, %f\n\r", time, bspdFlag);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, buf_len1, HAL_MAX_DELAY);
//
//	if (final_normalized >= normalizedTrigger) {
//		time++;
//		bspdFlag = 0.0;
//
//	//	buf_len1 = sprintf(buf1, "acc1_norm, acc2_norm, combined_norm:%f, %f, %f\n\r", ped1_normalized, ped2_normalized, final_normalized);
//		buf_len1 = sprintf(buf1, "time, bspd_flag:%f, %f\n\r", time, bspdFlag);
//		HAL_UART_Transmit(&huart2,(uint8_t *)buf1, buf_len1, HAL_MAX_DELAY);
//
//		if (time > timeThreshold) {
////			time = 0;
//			bspdFlag = 1.0;
//			buf_len1 = sprintf(buf1, "time, bspd_flag:%f, %f\n\r", time, bspdFlag);
//			HAL_UART_Transmit(&huart2,(uint8_t *)buf1, buf_len1, HAL_MAX_DELAY);
//			return 0.0;
//		}
//	}

	return final_normalized;
}

uint16_t FEB_Calculate_PSI1() {
	uint16_t brake_pres_1 =  FEB_Read_ADC(BRAKE_PRESS_1);
	float voltage_1 = brake_pres_1 / (3.3 / (3.3 + 1.69));
	float psi1 = ((voltage_1 - 500) / 4000) * 1000.0;
	return (uint16_t)(psi1);
}

uint16_t FEB_Calculate_PSI2() {
	uint16_t brake_pres_2 =  FEB_Read_ADC(BRAKE_PRESS_2);
    float voltage_2 = brake_pres_2 / (3.3 / (3.3 + 1.69));
	float psi2 = ((voltage_2 - 500) / 4000) * 1000.0;
	return (uint16_t)(psi2);;
}


void FEB_Normalized_CAN_sendBrake() {
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 5;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_BRAKE_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer. This might be incorrect. It's possible you have to do some bit shifting
//	memcpy(FEB_CAN_Tx_Data, &normalized_brake, sizeof(float));
	FEB_Normalized_update_Brake();
	memcpy(FEB_CAN_Tx_Data, &normalized_brake, sizeof(float)); // maybe?
	uint8_t converted_brake_val = (uint8_t)(normalized_brake * 100);

	FEB_CAN_Tx_Data[0] = converted_brake_val;
	FEB_CAN_Tx_Data[1] = (uint8_t)(psi1 & 0xFF);
	FEB_CAN_Tx_Data[2] = (uint8_t)((psi1 >> 8) & 0xFF);
	FEB_CAN_Tx_Data[3] = (uint8_t)(psi2 & 0xFF);
	FEB_CAN_Tx_Data[4] = (uint8_t)((psi2 >> 8) & 0xFF);


	//Debug to see the position of the Brake position sensor


	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}
//
	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//error - shutdown
		char buf[128];
		uint8_t buf_len;
		buf_len = sprintf(buf, "CAN MESSAGE FAIL TO SEND: %f\r\n", normalized_brake);
		HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);
	}

}




