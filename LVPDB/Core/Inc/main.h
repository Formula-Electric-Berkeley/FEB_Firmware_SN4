/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "FEB_Main.h"

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
#define SH_ALERT_Pin GPIO_PIN_0
#define SH_ALERT_GPIO_Port GPIOC
#define RF_ALERT_Pin GPIO_PIN_1
#define RF_ALERT_GPIO_Port GPIOC
#define RF_PG_Pin GPIO_PIN_2
#define RF_PG_GPIO_Port GPIOC
#define RF_EN_Pin GPIO_PIN_3
#define RF_EN_GPIO_Port GPIOC
#define SH_EN_Pin GPIO_PIN_0
#define SH_EN_GPIO_Port GPIOA
#define BL_SWITCH_Pin GPIO_PIN_1
#define BL_SWITCH_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define AS_EN_Pin GPIO_PIN_4
#define AS_EN_GPIO_Port GPIOA
#define AS_ALERT_Pin GPIO_PIN_5
#define AS_ALERT_GPIO_Port GPIOA
#define AS_PG_Pin GPIO_PIN_6
#define AS_PG_GPIO_Port GPIOA
#define SH_PG_Pin GPIO_PIN_7
#define SH_PG_GPIO_Port GPIOA
#define AF_PG_Pin GPIO_PIN_4
#define AF_PG_GPIO_Port GPIOC
#define AB_EN_Pin GPIO_PIN_5
#define AB_EN_GPIO_Port GPIOC
#define AB_ALERT_Pin GPIO_PIN_0
#define AB_ALERT_GPIO_Port GPIOB
#define AB_PG_Pin GPIO_PIN_1
#define AB_PG_GPIO_Port GPIOB
#define L_EN_Pin GPIO_PIN_2
#define L_EN_GPIO_Port GPIOB
#define L_ALERT_Pin GPIO_PIN_10
#define L_ALERT_GPIO_Port GPIOB
#define L_PG_Pin GPIO_PIN_14
#define L_PG_GPIO_Port GPIOB
#define CP_ALERT_Pin GPIO_PIN_15
#define CP_ALERT_GPIO_Port GPIOA
#define CP_PG_Pin GPIO_PIN_10
#define CP_PG_GPIO_Port GPIOC
#define CP_EN_Pin GPIO_PIN_11
#define CP_EN_GPIO_Port GPIOC
#define AF_ALERT_Pin GPIO_PIN_12
#define AF_ALERT_GPIO_Port GPIOC
#define AF_EN_Pin GPIO_PIN_5
#define AF_EN_GPIO_Port GPIOB
#define LV_PG_Pin GPIO_PIN_6
#define LV_PG_GPIO_Port GPIOB
#define LV_ALERT_Pin GPIO_PIN_7
#define LV_ALERT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
