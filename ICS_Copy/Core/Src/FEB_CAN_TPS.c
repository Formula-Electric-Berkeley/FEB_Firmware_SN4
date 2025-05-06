#include "FEB_CAN_TPS.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;
extern I2C_HandleTypeDef hi2c1;
extern uint8_t tps2482_i2c_addresses[1];
#define FLOAT_TO_UINT16_T(n)		((uint16_t)(n * 1000)) // for voltage (mV)
#define FLOAT_TO_INT16_T(n)			((int16_t)(n * 1000)) // for voltage (mV)
#define SIGN_MAGNITUDE(n)			(int16_t)((((n >> 15) & 0x01) == 1) ? -(n & 0x7FFF) : (n & 0x7FFF)) // for current reg

void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan) {
    uint16_t temp_voltage;
    TPS2482_Poll_Bus_Voltage(&hi2c1, tps2482_i2c_addresses, (uint8_t *) &temp_voltage, 1);
    uint16_t temp_current;
    TPS2482_Poll_Current(&hi2c1, tps2482_i2c_addresses, (uint8_t *) &temp_current, 1);

    uint16_t other_voltage;
    int16_t other_current;
    other_voltage = FLOAT_TO_UINT16_T(other_voltage * TPS2482_CONV_VBUS);
    other_current = FLOAT_TO_INT16_T(SIGN_MAGNITUDE(other_current) * TPS2482_CURRENT_LSB_EQ((double)(5)));

	FEB_CAN_Tx_Header.DLC = 4;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_PCU_TPS_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;



	// Configure FEB_CAN_Tx_Data
    FEB_CAN_Tx_Data[0] = other_voltage;
    FEB_CAN_Tx_Data[2] = other_current;

    // Write Code Here

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
