#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

// ******************************** Includes ********************************
#include "FEB_ADBMS6830B.h"
// ******************************** Functions ********************************

void FEB_Main_Setup(void);
void FEB_Task_ADBMS(void);
void FEB_Task_SM(void);
void FEB_Task_Charge(void);
void FEB_Task_Balance(void);
void FEB_Task_IVT(void);
void FEB_Task_CAN(void);
void FEB_Task_UART(void);

#endif /* INC_FEB_MAIN_H_ */
