// **************************************** Includes ****************************************
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "FEB_HW.h"
#include "FEB_UART_Transmit.h"

extern UART_HandleTypeDef huart2;

// **************************************** Global Variables ********************************

static uint8_t counter = 0;

// **************************************** Functions ****************************************
void FEB_ADBMS_UART_Transmit(accumulator_t* FEB_ACC) {
	int NUMLINES=4;
	for (uint8_t bank = 0; bank < FEB_NUM_BANKS; bank++) {
		char UART_line[NUMLINES][32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
		int offset[NUMLINES];
		offset[0]=sprintf((char*)(UART_line[0]),"|Bnk %d|",bank);
		offset[1]=sprintf((char*)(UART_line[1]),"|Vlt C|");
		offset[2]=sprintf((char*)(UART_line[2]),"|Vlt S|");
		offset[3]=sprintf((char*)(UART_line[3]),"|Tmp 1|");
		//offset[4]=sprintf((char*)(UART_line[4]),"|Tmp 2|");
		//offset[5]=sprintf((char*)(UART_line[5]),"|PWM  |");

		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK; cell++) {
			offset[0]+=sprintf(((char*)(UART_line[0]) + offset[0]), (cell>=10)?"Cell  %d|":"Cell   %d|",cell);
			offset[1]+=sprintf(((char*)(UART_line[1]) + offset[1]), "%.6f|",FEB_ACC->banks[bank].cells[cell].voltage_V);
			offset[2]+=sprintf(((char*)(UART_line[2]) + offset[2]), "%.6f|",FEB_ACC->banks[bank].cells[cell].voltage_S);
			offset[3]+=sprintf(((char*)(UART_line[3]) + offset[3]), "%.6f|",FEB_ACC->banks[bank].temp_sensor_readings_V[cell]); // @suppress("Float formatting support")
			//offset[4]+=sprintf(((char*)(UART_line[4]) + offset[4]), "%.6f|",FEB_ACC.banks[bank].temp_sensor_readings_V[cell]);
			//offset[5]+=sprintf(((char*)(UART_line[4]) + offset[4]), "%X|",FEB_ACC.banks[bank].temp_sensor_readings_V[cell+16]);
		}
		offset[NUMLINES-1]+=sprintf(((char*)(UART_line[NUMLINES-1]) + offset[NUMLINES-1]), "\n\r");
		for(int line=0;line<NUMLINES;line++){
			offset[line]+=sprintf(((char*)(UART_line[line]) + offset[line]), "\n\r") ;
			HAL_UART_Transmit(&huart2, (uint8_t*) UART_line[line], offset[line]+1, 100);
		}

	}
	char UART_line[128];
	size_t len =sprintf( (UART_line) , "------------------------------------------------------------------------------------------------\n\r\n\r");
	HAL_UART_Transmit(&huart2, (uint8_t*) UART_line, len+1, 100);
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
		case FEB_SM_ST_ESC:
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

