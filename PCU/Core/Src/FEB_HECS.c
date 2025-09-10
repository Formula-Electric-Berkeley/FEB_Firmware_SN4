// **************************************** Includes & External ****************************************

#include "FEB_HECS.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;



// **************************************** Configuration ****************************************

bool currHigh = false;


// **************************************** Functions ****************************************

uint16_t FEB_Read_ADC1(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig={0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	   Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	return (uint16_t)HAL_ADC_GetValue(&hadc1);

}

void FEB_HECS_update(){
	uint16_t current_HECS = FEB_Read_ADC1(HECS_SIGNAL);
	uint16_t current_ACC1 = FEB_Read_ADC1(ACC_PEDAL_1);
	uint16_t current_BRAKE1 = FEB_Read_ADC1(BRAKE_PRESS_1);
	uint16_t current_ACC2 = FEB_Read_ADC1(ACC_PEDAL_2);
	uint16_t current_BRAKE2 = FEB_Read_ADC1(BRAKE_PRESS_2);

	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "HECS Current:%d, %d, %d, %d, %d\n\r", current_HECS,current_ACC1,current_BRAKE1,current_ACC2,current_BRAKE2);
//	HAL_UART_Transmit(&huart2, buf, strlen(buf), HAL_MAX_DELAY);

	if (current_HECS >= HECS_CURRENT){ //checks if current is above value (to be determined)
		currHigh = true;
	}else{
		currHigh = false;
	}

}
