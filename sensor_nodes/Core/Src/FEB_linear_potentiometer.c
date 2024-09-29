#include <FEB_linear_potentiometer.h>
#define NUM_SAMPLES 20

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
uint16_t linear_potentiometer_reading;
char buf[164];


uint16_t get_linear_potentiometer_reading(void){

	uint32_t sum = 0;
	uint16_t linear_potentiometer_reading = 0;

	// Take multiple samples and average them
	for (int i = 0; i < NUM_SAMPLES; i++) {
		if (HAL_ADC_Start(&hadc1) != HAL_OK) {
					  // Start Error
					}
				// do displacement reading
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			sum += HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Stop(&hadc1);
	}


	linear_potentiometer_reading = sum / NUM_SAMPLES;
	sprintf(buf, "Potentiometer Measurement: %hu \r\n", linear_potentiometer_reading);
	HAL_UART_Transmit(&huart2, (uint8_t *) buf, strlen(buf), HAL_MAX_DELAY);
	return linear_potentiometer_reading;
}
