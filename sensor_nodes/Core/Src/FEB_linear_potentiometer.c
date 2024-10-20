#include <FEB_linear_potentiometer.h>
#define NUM_SAMPLES 20

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
uint32_t linear_potentiometer_reading;
char buf[164];


uint32_t get_linear_potentiometer_reading(void){

	if (HAL_ADC_Start(&hadc1) != HAL_OK) {
	  Error_Handler();
	}

	if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
		linear_potentiometer_reading = (uint32_t)HAL_ADC_GetValue(&hadc1);
	}

	sprintf(buf, "Potentiometer Measurement: %u\r\n", (unsigned) linear_potentiometer_reading);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
	return linear_potentiometer_reading;
}
