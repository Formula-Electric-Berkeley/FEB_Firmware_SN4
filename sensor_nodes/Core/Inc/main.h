/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define Thermocouple1_Pin GPIO_PIN_0
#define Thermocouple1_GPIO_Port GPIOC
#define Thermocouple2_Pin GPIO_PIN_1
#define Thermocouple2_GPIO_Port GPIOC
#define Thermocouple3_Pin GPIO_PIN_2
#define Thermocouple3_GPIO_Port GPIOC
#define CP2_OUT_Pin GPIO_PIN_3
#define CP2_OUT_GPIO_Port GPIOC
#define WSS_SIN_Pin GPIO_PIN_0
#define WSS_SIN_GPIO_Port GPIOA
#define WSS_COS_Pin GPIO_PIN_1
#define WSS_COS_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define CP1_OUT_Pin GPIO_PIN_4
#define CP1_OUT_GPIO_Port GPIOA
#define SG3_REF_Pin GPIO_PIN_5
#define SG3_REF_GPIO_Port GPIOA
#define LP_Wiper1_Pin GPIO_PIN_6
#define LP_Wiper1_GPIO_Port GPIOA
#define SG1_REF_Pin GPIO_PIN_4
#define SG1_REF_GPIO_Port GPIOC
#define SG4_REF_Pin GPIO_PIN_5
#define SG4_REF_GPIO_Port GPIOC
#define SG2_REF_Pin GPIO_PIN_0
#define SG2_REF_GPIO_Port GPIOB
#define LP_Wiper2_Pin GPIO_PIN_1
#define LP_Wiper2_GPIO_Port GPIOB
#define IMU_INT_Pin GPIO_PIN_2
#define IMU_INT_GPIO_Port GPIOB
#define IMU_RST_Pin GPIO_PIN_10
#define IMU_RST_GPIO_Port GPIOB
#define WSS_SIN1_Pin GPIO_PIN_6
#define WSS_SIN1_GPIO_Port GPIOC
#define WSS_COS1_Pin GPIO_PIN_7
#define WSS_COS1_GPIO_Port GPIOC
#define I2C1_SDA_Pin GPIO_PIN_9
#define I2C1_SDA_GPIO_Port GPIOC
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOA
#define Coolant_ReedSW_Pin GPIO_PIN_9
#define Coolant_ReedSW_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define Steering_DIR_Pin GPIO_PIN_15
#define Steering_DIR_GPIO_Port GPIOA
#define PGO_Pin GPIO_PIN_12
#define PGO_GPIO_Port GPIOC
#define GPS_EN_Pin GPIO_PIN_2
#define GPS_EN_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
