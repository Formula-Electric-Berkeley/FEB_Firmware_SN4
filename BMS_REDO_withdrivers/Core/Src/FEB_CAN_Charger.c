#include "FEB_CAN_Charger.h"
#include "FEB_SM.h"
#include "FEB_CAN_IDs.h"
#include "FEB_Config.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"

#define charger
#ifdef charger

#define FEB_CAN_ID_CHARGER_BMS 0x1806e5f4
#define FEB_CAN_ID_CHARGER_CCS 0x18FF50E5

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;
//extern osMutexId_t FEB_UART_LockHandle;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;

extern accumulator_t FEB_ACC;

typedef struct {
	uint16_t max_voltage_dV;		// Deci-volts
	uint16_t max_current_dA;		// Deci-amps
	uint8_t control;				// 0 (start charge), 1 (stop charge)
	bool done_charging;
} BMS_message_t;
typedef struct {
	uint16_t op_voltage_dV;			// Operating voltage
	uint16_t op_current_dA;			// Operating current
	uint8_t status;
	bool received;
} CCS_message_t;
static BMS_message_t BMS_message;
static CCS_message_t CCS_message;

static void charger_CAN_transmit(void);

bool received_msg = false;

static bool trickle_charge_enabled = false;
static uint32_t last_trickle_toggle_time = 0;
static bool charging_on = true;

/* ******** CAN Functions ******** */

uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t filter_bank) {
	CAN_FilterTypeDef CAN_filter_config;

	CAN_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	CAN_filter_config.FilterBank = filter_bank;
	CAN_filter_config.FilterFIFOAssignment = FIFO_Assignment;
	CAN_filter_config.FilterIdHigh = FEB_CAN_ID_CHARGER_CCS >> 13;
	CAN_filter_config.FilterIdLow = (FEB_CAN_ID_CHARGER_CCS & 0x1FFF) << 3;
	CAN_filter_config.FilterMaskIdHigh = 0xFFFF;
	CAN_filter_config.FilterMaskIdLow = 0xFFF8;
	CAN_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_filter_config.SlaveStartFilterBank = 27;

	if (HAL_CAN_ConfigFilter(hcan, &CAN_filter_config) != HAL_OK) {
//		FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}

	return ++filter_bank;
}

void FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]) {
	switch(pHeader->ExtId) {
	    case FEB_CAN_ID_CHARGER_CCS:
	    	CCS_message.op_voltage_dV = (uint16_t) (RxData[0] << 8) + RxData[1];
	    	CCS_message.op_current_dA = (uint16_t) (RxData[2] << 8) + RxData[3];
	    	CCS_message.status = RxData[4];
	    	CCS_message.received = true;
			break;
	}
}

static void charger_CAN_transmit(void) {
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.ExtId = FEB_CAN_ID_CHARGER_BMS;
	FEB_CAN_Tx_Header.IDE = CAN_ID_EXT;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = BMS_message.max_voltage_dV >> 8;
	FEB_CAN_Tx_Data[1] = BMS_message.max_voltage_dV & 0xFF;
	FEB_CAN_Tx_Data[2] = BMS_message.max_current_dA >> 8;
	FEB_CAN_Tx_Data[3] = BMS_message.max_current_dA & 0xFF;
	FEB_CAN_Tx_Data[4] = BMS_message.control;
	FEB_CAN_Tx_Data[5] = 0;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
//		FEB_SM_Set_Current_State(FEB_SM_ST_SHUTDOWN);
	}
}

// ******** Interface Functions ********

void FEB_CAN_Charger_Init(void) {
	BMS_message.max_voltage_dV = FEB_NBANKS * FEB_NUM_CELL_PER_BANK * (FEB_Config_Get_Cell_Max_Voltage_mV() * 1e-2);
	BMS_message.max_current_dA = CHARGE_CURRENT_dA;
	CCS_message.received = false;
	BMS_message.done_charging = false;
}

void FEB_CAN_Charger_Process(void) {
	if (FEB_SM_Get_Current_State() != FEB_SM_ST_CHARGING) {
		return;
	}

	uint16_t current_pack_voltage_dV = (uint16_t)(FEB_ACC.total_voltage_V * 10.0);
	
	if (current_pack_voltage_dV >= TRICKLE_CHARGE_START_VOLTAGE_dV) {
        trickle_charge_enabled = true;
    } else {
        trickle_charge_enabled = false;
        charging_on = true;
    }

	if (trickle_charge_enabled) {
        uint32_t current_time = HAL_GetTick();
        
        if ((current_time - last_trickle_toggle_time) >= TRICKLE_CHARGE_INTERVAL_MS) {
            charging_on = !charging_on;
            last_trickle_toggle_time = current_time;
        }

        if (charging_on) {
            BMS_message.max_voltage_dV = TRICKLE_CHARGE_MAX_VOLTAGE_dV;
            BMS_message.max_current_dA = TRICKLE_CHARGE_CURRENT_dA;
            BMS_message.control = 0;
        } else {
            BMS_message.max_voltage_dV = TRICKLE_CHARGE_MAX_VOLTAGE_dV;
            BMS_message.max_current_dA = 0;
            BMS_message.control = 0;
        }
    } else {
        BMS_message.max_voltage_dV = TRICKLE_CHARGE_MAX_VOLTAGE_dV;
        BMS_message.max_current_dA = CHARGE_CURRENT_dA;
        BMS_message.control = 0; 
    }

	charger_CAN_transmit();
}

void FEB_CAN_Charger_Start_Charge(void) {
	BMS_message.control = 0;
}

void FEB_CAN_Charger_Stop_Charge(void) {
	BMS_message.control = 1;
	BMS_message.done_charging = true;

}

bool FEB_CAN_Charger_Received(void){
	return CCS_message.received;
}

int8_t FEB_CAN_Charging_Status(void) {
	if (BMS_message.done_charging == true) {
		return 1;
	}

	if ( FEB_ACC.total_voltage_V >= FEB_CONFIG_PACK_HARD_MAX_VOLTAGE_V ) {
		return -1;
	}

	for ( size_t i = 0; i < FEB_NBANKS; ++i) {
		for ( size_t j = 0; j < FEB_NUM_CELL_PER_BANK; ++j) {
			if ( FEB_ACC.banks[i].cells[j].voltage_V * 1000 >= FEB_CONFIG_CELL_SOFT_MAX_VOLTAGE_mV ) {
				if ( FEB_ACC.banks[i].cells[j].voltage_V * 1000 >= FEB_CONFIG_CELL_HARD_MAX_VOLTAGE_mV ) {
					return -1;
				}
				return 1;
			}
			if ( FEB_ACC.banks[i].temp_sensor_readings_V[j] * 10  >= FEB_CONFIG_CELL_SOFT_MAX_TEMP_dC ) {
				if ( FEB_ACC.banks[i].temp_sensor_readings_V[j] * 10 >= FEB_CONFIG_CELL_HARD_MAX_TEMP_dC ) {
					return -1;
				}
				return 1;
			}
		}
	}

	return 0;
}
void FEB_CAN_Charger_Serial(void) {
	static char str[128];
	sprintf(str, "charger1 %d %d %d\n",
	BMS_message.max_voltage_dV, BMS_message.max_current_dA, BMS_message.control
	);

	//while (osMutexAcquire(FEB_UART_LockHandle, UINT32_MAX) != osOK);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
	//osMutexRelease(FEB_UART_LockHandle);
	sprintf(str, "charger2 %d %d %d\n",
	CCS_message.op_voltage_dV, CCS_message.op_current_dA, CCS_message.status
	);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);

}
void FEB_CAN_Charger_UART_Transmit(void) {
	static char str[128];

	// Data: max voltage, max current, control operating voltage, operating current, status bits
	sprintf(str, "Charger: Max_V: %d Max_A: %d Control: %d OP_V: %d OP_A: %d Status: %d Recieved: %d\n",
		BMS_message.max_voltage_dV, BMS_message.max_current_dA, BMS_message.control,
		CCS_message.op_voltage_dV, CCS_message.op_current_dA, CCS_message.status, CCS_message.received);

	//while (osMutexAcquire(FEB_UART_LockHandle, UINT32_MAX) != osOK);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
	//osMutexRelease(FEB_UART_LockHandle);
}
#endif
