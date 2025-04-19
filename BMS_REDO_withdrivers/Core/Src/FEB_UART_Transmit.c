// **************************************** Includes ****************************************
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "FEB_HW.h"
#include "FEB_UART_Transmit.h"
#include "FEB_IVT.h"

extern UART_HandleTypeDef huart2;

// **************************************** Global Variables ********************************

static uint8_t counter = 0;

// **************************************** Functions ****************************************
void FEB_ADBMS_UART_Transmit(accumulator_t* FEB_ACC) {
	int NUMLINES=5;
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		char UART_line[NUMLINES][32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
		int offset[NUMLINES];
		offset[0]=sprintf((char*)(UART_line[0]),"|Bnk %d|",bank+1);
		offset[1]=sprintf((char*)(UART_line[1]),"|Vlt C|");
		offset[2]=sprintf((char*)(UART_line[2]),"|Vlt S|");
		offset[3]=sprintf((char*)(UART_line[3]),"|Tmp 1|");
		offset[4]=sprintf((char*)(UART_line[4]),"|DsChg|");
		//offset[4]=sprintf((char*)(UART_line[5]),"|PWM  |");

		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK; cell++) {
			offset[0]+=sprintf(((char*)(UART_line[0]) + offset[0]), (cell>=9)?"Cell  %d|":"Cell   %d|",cell+1);
			float CV =FEB_ACC->banks[bank].cells[cell].voltage_V;
			offset[1]+=sprintf(((char*)(UART_line[1]) + offset[1]), CV>0?"%.6f|":"%.5f|",CV);
			float SV =FEB_ACC->banks[bank].cells[cell].voltage_S;
			offset[2]+=sprintf(((char*)(UART_line[2]) + offset[2]), SV>0?"%.6f|":"%.5f|",SV);
			offset[3]+=sprintf(((char*)(UART_line[3]) + offset[3]), "%.5f|",FEB_ACC->banks[bank].temp_sensor_readings_V[cell]); // @suppress("Float formatting support")
			offset[4]+=sprintf(((char*)(UART_line[4]) + offset[4]), FEB_ACC->banks[bank].cells[cell].discharging==1?"True  |":"False |");
			//offset[5]+=sprintf(((char*)(UART_line[4]) + offset[4]), "%X|",FEB_ACC.banks[bank].temp_sensor_readings_V[cell+16]);
		}

		offset[NUMLINES-1]+=sprintf(((char*)(UART_line[NUMLINES-1]) + offset[NUMLINES-1]), "\n\r");
		char Bank_line[NUMLINES*32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
		int index =0;
		for(int line=0;line<NUMLINES;line++){
			offset[line]+=sprintf(((char*)(UART_line[line]) + offset[line]), "\n\r") ;
			index+=sprintf(((char*)Bank_line)+index,UART_line[line]);
		}

		if(bank+1==FEB_NBANKS)index+=sprintf(((char*)Bank_line)+index,"Total voltage: %f \n\r",FEB_ACC->total_voltage_V);
		HAL_UART_Transmit(&huart2, (uint8_t*) Bank_line, index+1, 100);
	}
}
void FEB_MONITOR_UART_Transmit(accumulator_t*FEB_ACC){
	char UART_line[32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK; cell++) {
			sprintf(((char*)(UART_line)),"cell %d %d %.6f %.6f %d\n",
					bank,
					cell,
					FEB_ACC->banks[bank].cells[cell].voltage_V,
					FEB_ACC->banks[bank].temp_sensor_readings_V[cell],
					FEB_ACC->banks[bank].cells[cell].discharging
			);


			HAL_UART_Transmit(&huart2, (uint8_t*) UART_line, strlen(UART_line), 100);
		}
	}

}

void FEB_UART_Transmit_Process(void) {
	char str[2048];

	sprintf(str, "Counter: %u\n\r  ", counter);

	counter++;

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_PingPong_Members(void) {
	char str[2048];

	sprintf(str, "Members: %u\n\r", FEB_CAN_GET_PONGED());

	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}

void FEB_UART_Transmit_CAN_Error(int ErrorCode) {
	char str[2048];

	switch (ErrorCode){
	case 0:
		break;
	case FEB_CAN_EXCEPTION_TIMEOUT:
		sprintf(str, "CAN Timeout while waiting for empty mailboxes!\n");
		break;
	case FEB_CAN_EXCEPTION_TxFAIL:
		sprintf(str, "Transmission failed to send!\n");
		break;
	}
	HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 100);
}

//Output current state, relay state, and GPIO sense.
void FEB_SM_UART_Transmit(void) {
	if (!FEB_SM_ST_DEBUG)
		return;

	FEB_SM_ST_t state = FEB_SM_Get_Current_State();
	char* state_str;
	switch(state) {
		case FEB_SM_ST_BOOT:
			state_str = "Boot";
			break;
		case FEB_SM_ST_LV:
			state_str = "LV Power";
			break;
		case FEB_SM_ST_HEALTH_CHECK:
			state_str = "ESC Complete";
			break;
		case FEB_SM_ST_PRECHARGE:
			state_str = "PRECHARGE";
			break;
		case FEB_SM_ST_ENERGIZED:
			state_str = "Energized";
			break;
		case FEB_SM_ST_DRIVE:
			state_str = "Drive";
			break;
		case FEB_SM_ST_FREE:
			state_str = "Accumulator Free";
			break;
		case FEB_SM_ST_CHARGING:
			state_str = "Charging";
			break;
		case FEB_SM_ST_BALANCE:
			state_str = "Balancing";
			break;
		case FEB_SM_ST_FAULT_BMS:
			state_str = "BMS Fault";
			break;
		case FEB_SM_ST_FAULT_IMD:
			state_str = "IMD Fault";
			break;
		case FEB_SM_ST_FAULT_BSPD:
			state_str = "BSPD Fault";
			break;
		default:
			state_str= "undef";
			break;
	}

	//while (osMutexAcquire(FEB_SM_LockHandle, UINT32_MAX) != osOK);
	FEB_Relay_State bms_shutdown_relay = FEB_PIN_RD(PN_SHS_IN);//FEB_Hw_Get_BMS_Shutdown_Relay();
	FEB_Relay_State air_plus_relay = FEB_PIN_RD(PN_PC_AIR);//FEB_Hw_Get_AIR_Plus_Relay();
	FEB_Relay_State precharge_relay = FEB_PIN_RD(PN_PC_REL);//FEB_Hw_Get_Precharge_Relay();
	FEB_Relay_State air_minus_sense = FEB_PIN_RD(PN_AIRM_SENSE);//FEB_Hw_AIR_Minus_Sense();
	FEB_Relay_State air_plus_sense = FEB_PIN_RD(PN_AIRP_SENSE);//FEB_Hw_AIR_Plus_Sense();
	FEB_Relay_State bms_shutdown_sense = FEB_PIN_RD(PN_SHS_TSMS);//FEB_Hw_BMS_Shutdown_Sense();
	FEB_Relay_State imd_shutdown_sense = FEB_PIN_RD(PN_SHS_IMD);//FEB_Hw_IMD_Shutdown_Sense();
	//bool r2d = FEB_CAN_ICS_Ready_To_Drive();
	//uint8_t ics = FEB_CAN_ICS_Data();
	//osMutexRelease(FEB_SM_LockHandle);

	static char str[128];
	sprintf(str, "state %s %d %d %d %d %d %d %d\n\r", state_str,
			bms_shutdown_relay, air_plus_relay, precharge_relay,
			air_minus_sense, air_plus_sense,
			bms_shutdown_sense, imd_shutdown_sense);

	//while (osMutexAcquire(FEB_UART_LockHandle, UINT32_MAX) != osOK);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
	//osMutexRelease(FEB_UART_LockHandle);
}

void FEB_IVT_V1_Transmit(){
	int32_t voltage_value = FEB_IVT_V1_Voltage();
	static char str[128];
	sprintf(str,"IVT V1 Voltage: %d \n\r", voltage_value);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 500);

}

void FEB_SM_State_Transmit(){
	FEB_SM_ST_t state = FEB_SM_Get_Current_State();
	static char str[128];

	switch(state){
		case FEB_SM_ST_BOOT:
			sprintf(str,"State: Boot\n\r");
			break;
		case FEB_SM_ST_LV:
			sprintf(str,"State: LV\n\r");
			break;
		case FEB_SM_ST_HEALTH_CHECK:
			sprintf(str,"State: ESC\n\r");
			break;
		case FEB_SM_ST_PRECHARGE:
			sprintf(str,"State: Precharge\n\r");
			break;
		case FEB_SM_ST_ENERGIZED:
			sprintf(str,"State: energized\n\r");
			break;
		case FEB_SM_ST_DRIVE:
			sprintf(str,"State: drive\n\r");
			break;
		case FEB_SM_ST_FREE:
			sprintf(str,"State: free\n\r");
			break;
		case FEB_SM_ST_CHARGING:
			sprintf(str,"State: charging\n\r");
			break;
		case FEB_SM_ST_BALANCE:
			sprintf(str,"State: balance\n\r");
			break;
		case FEB_SM_ST_FAULT_BMS:
			sprintf(str,"State: fault BMS\n\r");
			break;
		case FEB_SM_ST_FAULT_BSPD:
			sprintf(str,"State: fault BSPD\n\r");
			break;
		case FEB_SM_ST_FAULT_IMD:
			sprintf(str,"State: fault IMD\n\r");
			break;
		case FEB_SM_ST_FAULT_CHARGING:
			sprintf(str,"State: fault charging\n\r");
			break;
		case FEB_SM_ST_DEFAULT:
			sprintf(str,"State: default\n\r");
			break;
		default:
			sprintf(str,"State: NA\n\r");

			break;
	}
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

void FEB_Transmit_AIR_Status(){
	static char str1[128];
	static char str2[128];
	static char str3[128];
	static char str4[128];
	static char str5[128];
	static char str6[128];

	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_4)){
		sprintf(str1,"AIRMINUS SET\n\r");
	}else{
		sprintf(str1,"AIRMINUS RESET\n\r");
	}


	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5)){
		sprintf(str2,"AIRPLUS SET\n\r");
	} else {
		sprintf(str2,"AIRPLUS RESET\n\r");
	}

	sprintf(str3,"BMS SHUTDOWN: %d\n\r", FEB_PIN_RD(PN_BMS_A));
	sprintf(str4,"IMD SHUTDOWN: %d\n\r", FEB_PIN_RD(PN_SHS_IMD));
	sprintf(str5,"RELAY STATE: %d\n\r", FEB_PIN_RD(PN_PC_REL));

	sprintf(str6,"——————————————————————————————————————\n\r");

	HAL_UART_Transmit(&huart2, (uint8_t*) str1, strlen(str1), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str2, strlen(str2), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str5, strlen(str5), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str3, strlen(str3), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str4, strlen(str4), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str6, strlen(str6), 100);
}
