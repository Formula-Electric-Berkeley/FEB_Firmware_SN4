#ifndef INC_FEB_SM_H_
#define INC_FEB_SM_H_

// ******************************** Includes ********************************

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include "FEB_CAN.h"
#include "FEB_Config.h"



// ************************ Task Queues ***************
void FEB_SM_Init(void);
FEB_SM_ST_t FEB_SM_Get_Current_State(void);
void FEB_SM_Transition(FEB_SM_ST_t next_state);
void FEB_SM_Process(void);



/* Faults
CAN Initialization fails
Under/Over cell voltage
Under/Over cell temperature
Under/Over pack current
*/

#endif /* INC_FEB_SM_H_ */
