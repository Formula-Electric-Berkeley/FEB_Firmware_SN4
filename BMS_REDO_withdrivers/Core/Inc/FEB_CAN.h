#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_


// **************************************** Includes ****************************************

#include "FEB_CAN_IDs.h"
#include "FEB_Config.h"
#include "FEB_Task_Queue.h"
#include "FEB_SM.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"

//***************************************** Structs *****************************************
typedef enum {
	DEV_IND_IVM,
	DEV_IND_DASH,
	DEV_IND_PCU,
	DEV_IND_DCU,
	DEV_IND_SN,
	DEV_IND_CHARGER,
} FEB_DEV_INDEX;


typedef struct {
	uint8_t FAck; //Failed Acknowledgments
	uint8_t LaOn; //CAN SM Reports since last update
} FEB_CAN_DEV;



// **************************************** Constants ***************************************
#define FEB_CAN_PINGPONG_MODE 1
#define FEB_CAN_TIMEOUT_MS 2000
#define FEB_CAN_EXCEPTION_TIMEOUT 1000
#define FEB_CAN_EXCEPTION_TxFAIL 1001

// **************************************** Functions ***************************************
//FEB_Task_Header_t FEB_CAN_Tasks;

void FEB_CAN_Init(void);
void FEB_CAN_Filter_Config(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);
void FEB_SM_CAN_Transmit(void);
void FEB_CAN_PING(void);
void FEB_CAN_PONG(CAN_RxHeaderTypeDef*, uint8_t[]);
uint8_t FEB_CAN_GET_PONGED(void);
uint8_t FEB_CAN_PINGPONG_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

#endif /* INC_FEB_CAN_H_ */
