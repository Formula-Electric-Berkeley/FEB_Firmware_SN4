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
#include "FEB_CircularBuffer.h"
#include "FEB_CAN.h"
#include "FEB_XBee.h"
#include "FEB_CAN_Heartbeat.h"
#include "FEB_CAN_TPS.h"
#include "TPS2482.h"
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
#define XB_NRTS_Pin GPIO_PIN_1
#define XB_NRTS_GPIO_Port GPIOC
#define XB_DTR_Pin GPIO_PIN_2
#define XB_DTR_GPIO_Port GPIOC
#define mSD_CS_Pin GPIO_PIN_1
#define mSD_CS_GPIO_Port GPIOA
#define PG_Pin GPIO_PIN_4
#define PG_GPIO_Port GPIOA
#define mSD_SCK_Pin GPIO_PIN_5
#define mSD_SCK_GPIO_Port GPIOA
#define mSD_MISO_Pin GPIO_PIN_6
#define mSD_MISO_GPIO_Port GPIOA
#define mSD_MOSI_Pin GPIO_PIN_7
#define mSD_MOSI_GPIO_Port GPIOA
#define XB_MOSI_Pin GPIO_PIN_0
#define XB_MOSI_GPIO_Port GPIOB
#define XB_CTS_Pin GPIO_PIN_2
#define XB_CTS_GPIO_Port GPIOB
#define XB_RST_Pin GPIO_PIN_10
#define XB_RST_GPIO_Port GPIOB
#define XB_ON_Pin GPIO_PIN_6
#define XB_ON_GPIO_Port GPIOC
#define XB_CS_Pin GPIO_PIN_15
#define XB_CS_GPIO_Port GPIOA
#define XB_SCK_Pin GPIO_PIN_10
#define XB_SCK_GPIO_Port GPIOC
#define XB_MISO_Pin GPIO_PIN_11
#define XB_MISO_GPIO_Port GPIOC
#define XB_ATTN_Pin GPIO_PIN_12
#define XB_ATTN_GPIO_Port GPIOC
#define TPS_PG_Pin GPIO_PIN_6
#define TPS_PG_GPIO_Port GPIOB
#define TPS_ALERT_Pin GPIO_PIN_7
#define TPS_ALERT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
