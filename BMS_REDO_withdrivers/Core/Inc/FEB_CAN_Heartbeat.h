#ifndef INC_FEB_Heartbeat_CAN_H_
#define INC_FEB_Heartbeat_CAN_H_

// **************************************** Includes ****************************************

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN_IDs.h"
#include "FEB_Const.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_SM.h"

// **************************************** Constants ***************************************
#define FEB_FACK_THRESHOLD      3

//***************************************** Structs *****************************************
typedef enum {
	DEV_IND_IVM,
	DEV_IND_DASH,
	DEV_IND_PCU,
	DEV_IND_DCU,
	DEV_IND_FSN,
	DEV_IND_RSN,
	DEV_IND_LVPDB,
	DEV_IND_DART1,
	DEV_IND_DART2,
	DEV_IND_ALL,
} FEB_DEV_INDEX;

typedef struct {
	uint64_t last_received;
	uint8_t FAck; //Failed Acknowledgments
	uint8_t LaOn; // number of CAN SM Reports since last struct update
    bool initialized;
} FEB_CAN_DEV;

typedef enum {
    INITIALIZED,
    DISCONNECTED,
    CONNECTED,
} FEB_DEV_STATUS;

// **************************************** Functions ***************************************

uint8_t FEB_CAN_Heartbeat_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_Heartbeat_Store_Msg(CAN_RxHeaderTypeDef* rx_header, uint8_t RxData[]);
FEB_DEV_STATUS FEB_GetStatus(uint8_t device_index);
FEB_DEV_STATUS FEB_COMBINED_STATUS();

#endif /* INC_FEB_Heartbeat_CAN_H_ */



