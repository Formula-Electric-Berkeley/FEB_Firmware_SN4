#include <FEB_linear_potentiometer.h>
#define NUM_SAMPLES 20

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

uint32_t linear_potentiometer_reading;
float linear_potentiometer_value;
char buf[164];
float adc_resolution = 4095;
float linear_potentiometer_length = 75; //mm


uint32_t get_linear_potentiometer_reading(void){

	if (HAL_ADC_Start(&hadc1) != HAL_OK) {
	  Error_Handler();
	}

	if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
		linear_potentiometer_reading = (uint32_t)HAL_ADC_GetValue(&hadc1);
	}

	linear_potentiometer_value = linear_potentiometer_reading / adc_resolution * linear_potentiometer_length;


	sprintf(buf, "Potentiometer Measurement: %d\r\n", (int) linear_potentiometer_value);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
	return linear_potentiometer_reading;
}
