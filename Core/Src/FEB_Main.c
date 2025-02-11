/*
 * FEB_Main.c
 *
 * Author: Dorukhan User
 */
#include "FEB_Main.h"
#include "AirTemp.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC1_Init(void);
void MX_USART2_UART_Init(void);
void Send_UART(char *message);

void FEB_Setup() {
    AirTemp_Init();
}

void FEB_Loop() {
    char msg[50];

    while (1) {
        float temperature = Read_Temperature();
        sprintf(msg, "Temperature: %.2f C\r\n", temperature);
        Send_UART(msg);
        HAL_Delay(1000);
    }
}

void Send_UART(char *message) {
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

