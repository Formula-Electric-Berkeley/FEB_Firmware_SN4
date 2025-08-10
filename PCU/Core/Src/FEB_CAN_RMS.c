// ********************************** Includes & External **********************************

#include "FEB_CAN_RMS.h"
#include <math.h>

#define FEB_CAN_ID_RMS_VOLTAGE 0xa7
#define FEB_CAN_ID_RMS_MOTOR 0xa5

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;

// *********************************** Struct ************************************

struct {
  int16_t torque;
  uint8_t enabled;
} RMSControl;

typedef struct RMS_MESSAGE_TYPE {
    int16_t HV_Bus_Voltage;
    int16_t Motor_Speed;
} RMS_MESSAGE_TYPE;
RMS_MESSAGE_TYPE RMS_MESSAGE;

extern char buf[128];
extern uint8_t buf_len;

// ********************************** Functions **********************************

// ***** SET UP ****

void FEB_CAN_RMS_Setup(void){
	 RMSControl.enabled = 0;
	 RMSControl.torque= 0.0;
}

void FEB_CAN_RMS_Process(void){
	if (!RMSControl.enabled){
//		FEB_CAN_RMS_Init();
		RMSControl.enabled = 1;
	}
}

void FEB_CAN_RMS_Disable(void){
	RMSControl.enabled = 0;
}

#define min(x1, x2) x1 < x2 ? x1 : x2;


// ****** REGEN FUNCTIONS ****

// Returns maximum torque available within 20A limit of the cells
// V_acc * 20 * efficiency = torque / w
float FEB_getElecMaxRegenTorque(){
	float accumulator_voltage = min(INIT_VOLTAGE, (RMS_MESSAGE.HV_Bus_Voltage-50) / 10); // TODO: consider reading from IVT
	float motor_speed_rads = RMS_MESSAGE.Motor_Speed * RPM_TO_RAD_S;

	float maxTorque = min(MAX_TORQUE_REGEN, (accumulator_voltage * PEAK_CURRENT_REGEN) / motor_speed_rads);
	return maxTorque;
}

// Step function at from 0 to 1 at FADE_SPEED
float FEB_regen_filter_Speed(float unfiltered_regen_torque){
    float motor_speed_rpm = RMS_MESSAGE.Motor_Speed; 

    if (motor_speed_rpm < FADE_SPEED_RPM)
    {
        return 0;
    }
    else
    {
        return unfiltered_regen_torque;
    }
}

// Saturated linear function between
// (START_REGEN_SOC, 0) and (MAX_REGEN_SOC, 1)
float FEB_regen_filter_SOC(float unfiltered_regen_torque){
	float state_of_charge = 1; // FEB_BMS_getSOC(); // TODO for Evan, uncomment and use SOC

	// m = (y_1 - y_0) / (x_1 - x_0)
	float slope = (1 - 0) / (MAX_REGEN_SOC - START_REGEN_SOC);
	// y - y_0 = m (x - x_0)
	float k_SOC = slope * (state_of_charge - START_REGEN_SOC);
    
	// Saturate between 0 and 1
	if (k_SOC > 1)
	{
		k_SOC = 1;
	}
	if (k_SOC < 0)
	{
		return 0;
	}
	return k_SOC * unfiltered_regen_torque; // be wary of typing (float vs int)
}

// Consider linear function similar to the above if driver doesn't like the exponential
// This will depend on how fast cells heat up
// Function with a vertical asymptote at 45 deg C
float FEB_regen_filter_Temp(float unfiltered_regen_torque){
	float hottest_cell_temp_C = 25; //(); // TODO
	float e = 2.71828;
	float exponent = TEMP_FILTER_SHARPNESS * (hottest_cell_temp_C - MAX_CELL_TEMP);
	float k_temp = 1 - pow(e, exponent);
	if (k_temp < 0)
	{
		return 0;
	}
	return k_temp * unfiltered_regen_torque;
}

// Wrapper function for various filters
float FEB_regen_filter(float regen_torque_max){
	float filtered_regen_torque = regen_torque_max;
	filtered_regen_torque = FEB_regen_filter_Speed(filtered_regen_torque);
	filtered_regen_torque = FEB_regen_filter_SOC(filtered_regen_torque);
	filtered_regen_torque = FEB_regen_filter_Temp(filtered_regen_torque);
	return filtered_regen_torque * USER_REGEN_FILTER;
}

// **** TORQUE FUNCTIONS****

// Returns MAGNITUDE of regen torque
float FEB_CAN_RMS_getFilteredTorque_Regen(void){
	float present_regen_max = FEB_getElecMaxRegenTorque();
	return FEB_regen_filter(present_regen_max);
}



void FEB_CAN_RMS_Disable_undervolt(void){
	//Clear fault in case inverter is powered before disable command sent
	uint8_t fault_clear_addr = 140;
	uint8_t fault_clear_data = 0;

	//-----Transmit Stuff Below-----
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_RMS_PARAM_FRAME_ID; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
		// param msg format (little-endian):
		// 0,1: param addr
		// 2: r/w cmd
		// 3: NA
		// 4,5: data
		// 6,7: NA
	FEB_CAN_Tx_Data[0] = fault_clear_addr;
	FEB_CAN_Tx_Data[1] = 0;
	FEB_CAN_Tx_Data[2] = 1;
	FEB_CAN_Tx_Data[3] = 0;
	FEB_CAN_Tx_Data[4] = fault_clear_data;
	FEB_CAN_Tx_Data[5] = 0;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//Shutdown error
	}

}

// Essentially we want our voltage to never drop below 400 to be safe (~2.85V per cell) 
// To keep 400 as our floor setpoint, we will derate our torque limit based on voltage
// We will control this by derating our PEAK_CURRENT value. 
// Based on data at 510 V, we see that the voltage drops about 62V when commanding 65 A of current
// This means we will assume the pack resistance is about 1 Ohm
// Note this will likely be steeper as we approach lower SOC, but these cell dynamics are hopefully
// negligible due to the heavy current limiting and the ohmic losses of the accumulator
// Linear interpolation between (460V, 60/60A) and (410V, 10/60A), where PEAK_CURRENT is 60A
float FEB_get_peak_current_delimiter()
{
	float accumulator_voltage = (RMS_MESSAGE.HV_Bus_Voltage-50.0) / 10.0;
	float estimated_voltage_drop_at_peak = PEAK_CURRENT;
	float start_derating_voltage = 400.0 + PEAK_CURRENT; // Assume R_acc = 1ohm
	// Note: Comments are based on start_derating_voltage = 460V and PEAK_CURRENT = 60

	// Saturate outside of 460 and 410
	if (accumulator_voltage > start_derating_voltage)
	{
		return 1;
	}
	if (accumulator_voltage <= 410)
	{
		return (10.0 / PEAK_CURRENT); // limit to only 10A 
	}

	//      m   = (        y_1           -              y_0)              / (x_1 -          x_0)
	float slope = ((10.0 / PEAK_CURRENT) - (PEAK_CURRENT / PEAK_CURRENT)) / (410.0 - (start_derating_voltage));
	//      y     =   m     (       x            -          x_0          ) + y_0
	float derater = slope * (accumulator_voltage - start_derating_voltage) + 1.0;

	return derater;   
}

float FEB_CAN_RMS_getMaxTorque(void){
	// float accumulator_voltage = min(INIT_VOLTAGE, (RMS_MESSAGE.HV_Bus_Voltage-50) / 10); // TODO: consider reading from IVT
	float motor_speed = RMS_MESSAGE.Motor_Speed * RPM_TO_RAD_S;
	float peak_current_limited = PEAK_CURRENT * FEB_get_peak_current_delimiter();
	float power_capped = peak_current_limited * 400.0; // Cap power to 24kW (i.e. our min voltage)
 	// If speed is less than 15, we should command max torque
  	// This catches divide by 0 errors and also negative speeds (which may create very high negative torque values)
	if (motor_speed < 15) {
		return MAX_TORQUE;
	}
	float maxTorque = min(MAX_TORQUE, (power_capped) / motor_speed);
	return maxTorque;
}

void FEB_CAN_RMS_Torque(void){
	FEB_SM_ST_t current_BMS_state = FEB_CAN_BMS_getState(); // TODO: FOR ALEX
	float accPos = FEB_Normalized_Acc_Pedals();
	float brkPos = FEB_Normalized_getBrake();

	RMSControl.torque = 10 * accPos * FEB_CAN_RMS_getMaxTorque(); // temp


	FEB_CAN_RMS_Transmit_updateTorque();
}

void FEB_CAN_ACC(void) {
	uint16_t acc_pedal_1 = FEB_Read_ADC(ACC_PEDAL_1);
	uint16_t acc_pedal_2 = FEB_Read_ADC(ACC_PEDAL_2);

	FEB_CAN_RMS_Transmit_updateAcc(acc_pedal_1, acc_pedal_2);
}

void FEB_CAN_RMS_AUTO_Torque(uint16_t torque){

	RMSControl.torque = torque; // temp

	FEB_CAN_RMS_Transmit_updateTorque();
}

// ***** CAN FUNCS ***
void FEB_CAN_RMS_Init(void){

	for(int i = 0; i < 10; i++){
		FEB_CAN_RMS_Transmit_paramSafety();
		HAL_Delay(10);
	}

	for(int i = 0; i < 10; i++){
		FEB_CAN_RMS_Disable_undervolt();
		HAL_Delay(10);
	}


	// send disable command to remove lockout
	for (int i = 0; i < 10; i++) {
		FEB_CAN_RMS_Transmit_commDisable();
		HAL_Delay(10);
	}

	// Select CAN msg to broadcast
	FEB_CAN_RMS_Transmit_paramBroadcast();
}

void FEB_CAN_RMS_Transmit_updateTorque(void) { //TODO: Create Custom Transmit function and update below call
  //uint8_t message_data[8] = {RMSControl.torque & 0xFF, RMSControl.torque >> 8, 0, 0, 0, RMSControl.enabled, 0, 0};
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = 0xc0; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = (uint8_t)RMSControl.torque & 0xFF;
	FEB_CAN_Tx_Data[1] = (uint8_t)(RMSControl.torque >> 8) & 0xFF;
	FEB_CAN_Tx_Data[2] = 0;
	FEB_CAN_Tx_Data[3] = 0;
	FEB_CAN_Tx_Data[4] = 1;
	FEB_CAN_Tx_Data[5] = RMSControl.enabled;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}

void FEB_CAN_RMS_Transmit_updateAcc(uint16_t acc0, uint16_t acc1) {
	FEB_CAN_Tx_Header.DLC = 4;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_PCU_RAW_ACC_FRAME_ID; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = (uint8_t)acc0 & 0xFF;
	FEB_CAN_Tx_Data[1] = (uint8_t)(acc0 >> 8) & 0xFF;
	FEB_CAN_Tx_Data[2] = (uint8_t)acc1 & 0xFF;
	FEB_CAN_Tx_Data[3] = (uint8_t)(acc1 >> 8) & 0xFF;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}

uint8_t FEB_CAN_RMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_ID_RMS_VOLTAGE, FEB_CAN_ID_RMS_MOTOR};

	for (uint8_t i = 0; i < 2; i++) {
		CAN_FilterTypeDef filter_config;

	    // Standard CAN - 2.0A - 11 bit
	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = filter_bank;
		filter_config.FilterFIFOAssignment = FIFO_assignment;
		filter_config.FilterIdHigh = ids[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0xFFE0;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;
	    filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
			//Code Error - shutdown
		}
	}

	return filter_bank;
}

//TODO: WORK IN PROGRESS

void FEB_CAN_RMS_Transmit_paramSafety(void){
	//Clear fault in case inverter is powered before disable command sent
	uint8_t fault_clear_addr = 20;
	uint8_t fault_clear_data = 0;

	//-----Transmit Stuff Below-----
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_RMS_PARAM_FRAME_ID; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
		// param msg format (little-endian):
		// 0,1: param addr
		// 2: r/w cmd
		// 3: NA
		// 4,5: data
		// 6,7: NA
	FEB_CAN_Tx_Data[0] = fault_clear_addr;
	FEB_CAN_Tx_Data[1] = 0;
	FEB_CAN_Tx_Data[2] = 1;
	FEB_CAN_Tx_Data[3] = 0;
	FEB_CAN_Tx_Data[4] = fault_clear_data;
	FEB_CAN_Tx_Data[5] = 0;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//Shutdown error
	}

}

void FEB_CAN_RMS_Transmit_commDisable(void){
	//Selects CAN msg to broadcast



	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_RMS_PARAM_FRAME_ID; //ID for sending command messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
		// param msg format (little-endian):
		// 0,1: param addr
		// 2: r/w cmd
		// 3: NA
		// 4,5: data
		// 6,7: NA
	FEB_CAN_Tx_Data[0] = 0;
	FEB_CAN_Tx_Data[1] = 0;
	FEB_CAN_Tx_Data[2] = 0;
	FEB_CAN_Tx_Data[3] = 0;
	FEB_CAN_Tx_Data[4] = 0;
	FEB_CAN_Tx_Data[5] = 0;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}

}


void FEB_CAN_RMS_Transmit_paramBroadcast(void){

	uint8_t param_addr = 148;
	uint8_t CAN_active_msg_byte4 = 0b10100000; // motor position, input voltage
	uint8_t CAN_active_msg_byte5 = 0b00010101; // flux info (dq axes), torque/timer info, internal states

	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_RMS_PARAM_FRAME_ID; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = param_addr;
	FEB_CAN_Tx_Data[1] = 0;
	FEB_CAN_Tx_Data[2] = 1;
	FEB_CAN_Tx_Data[3] = 0;
	FEB_CAN_Tx_Data[4] = CAN_active_msg_byte4;
	FEB_CAN_Tx_Data[5] = CAN_active_msg_byte5;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}


void FEB_CAN_RMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
	switch (pHeader -> StdId){
		case FEB_CAN_ID_RMS_VOLTAGE:
			memcpy(&(RMS_MESSAGE.HV_Bus_Voltage), RxData, 2);
			break;
		case FEB_CAN_ID_RMS_MOTOR:
			memcpy(&(RMS_MESSAGE.Motor_Speed), RxData+2, 2);
			break;
	}
}
