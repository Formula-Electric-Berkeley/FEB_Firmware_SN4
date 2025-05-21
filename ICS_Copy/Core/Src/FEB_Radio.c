// **************************************** Includes ****************************************

#include "FEB_Radio.h"

extern UART_HandleTypeDef huart6;

extern ICS_UI_Values_t ICS_UI_Values;

// **************************************** Functions ****************************************

void FEB_Radio_Setup(void) {

}

void FEB_Radio_Transmit(void) {
	char data[30] = "";

	char temp_str[12];
	sprintf(temp_str, "T:%f", ICS_UI_Values.max_acc_temp);
	strcat(data, temp_str);
	strcat(data, "\n");

	char voltage_str[12];
	sprintf(voltage_str, "V:%f", ICS_UI_Values.ivt_voltage);
	strcat(data, voltage_str);
	strcat(data, "\n");

	char state_str[6];
	sprintf(state_str, "S:%d", ICS_UI_Values.bms_state);
	strcat(data, state_str);
	strcat(data, "\n");

	HAL_UART_Transmit(&huart6, (uint8_t *) data, sizeof(data), 100);
}
