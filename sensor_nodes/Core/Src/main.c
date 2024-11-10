/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdbool.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
	uint8_t zmco;
	uint16_t zpos;
	uint16_t mpos;
	uint16_t mang;
	uint16_t conf;
	uint8_t i2c_addr;
	uint8_t i2c_updt;
	uint16_t raw_angle;
	uint16_t angle;
	uint8_t status;
	uint8_t agc;
	uint16_t magnitude;

} AS5600L_Data;

typedef struct {
	uint32_t can_counter;
	uint16_t flags;
	uint16_t angle;

} CAN_Data;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// FEB constants
#define FEB_CAN_ID_Steer_Wheel_ENC \
								(uint16_t) 0xAA // Encoder-specific ID

// AS5600L Register Table
#define AS5600L_CONFIG_ZMCO 	(uint8_t) 0x00
#define AS5600L_CONFIG_ZPOS 	(uint8_t) 0x01 	// Start POS
#define AS5600L_CONFIG_MPOS 	(uint8_t) 0x03 	// End POS
#define AS5600L_CONFIG_MANG 	(uint8_t) 0x05 	// Angular Range (>18 deg)
#define AS5600L_CONFIG_CONF 	(uint8_t) 0x07
#define AS5600L_CONFIG_I2CADDR 	(uint8_t) 0x20
#define AS5600L_CONFIG_I2CUPDT 	(uint8_t) 0x21

#define AS5600L_OUT_ANG_RAW 	(uint8_t) 0x0C
#define AS5600L_OUT_ANG 		(uint8_t) 0x0E

#define AS5600L_STATUS_STATUS 	(uint8_t) 0x0B
#define AS5600L_STATUS_AGC 		(uint8_t) 0x1A
#define AS5600L_STATUS_MAG 		(uint8_t) 0x1B

#define AS5600L_BURN	 		(uint8_t) 0xFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static const uint16_t conf_val = 0x0300; 				// least filtering possible

static const uint16_t conf_pm_mask 		= 0x03; 		// Power Mode mask
static const uint16_t conf_hyst_mask 	= 0x03 << 2; 	// Hysteresis mask
static const uint16_t conf_outs_mask 	= 0x03 << 4;	// Output Stage mask
static const uint16_t conf_pwmf_mask 	= 0x03 << 6; 	// PWM Frequency mask
static const uint16_t conf_sf_mask 		= 0x03 << 8; 	// Slow Filter mask
static const uint16_t conf_fth_mask 	= 0x07 << 10; 	// Fast Filter Threshold mask
static const uint16_t conf_wd_mask 		= 0x01 << 13; 	// Watch Dog mask

static const uint8_t status_mag_mask 	= 0x07 << 3; 	// Magnet Detection bits

static uint8_t conf_i2c_addr = 0x40 << 1; // 0x40 by default
static AS5600L_Data i2c_data;
static CAN_Data can_data;
static uint8_t canTx[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM6_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
/* USER CODE BEGIN PFP */
static bool Steer_ENC_I2C_Init(void);
static void Steer_ENC_I2C_Read(void);
static void Steer_ENC_I2C_Full_Read(void);
static void Steer_ENC_CAN_Message(uint8_t *canTx);
static void Steer_ENC_CAN_Transmit(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C3_Init();
  MX_TIM6_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  /* USER CODE BEGIN 2 */
  Steer_ENC_I2C_Init();
  Steer_ENC_I2C_Full_Read(); // Do an initial full read
  HAL_TIM_Base_Start_IT(&htim6);  // Start 1 ms timer
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 16;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 128;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 84-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ENC_PGO_GPIO_Port, ENC_PGO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ENC_DIR_GPIO_Port, ENC_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_PGO_Pin */
  GPIO_InitStruct.Pin = ENC_PGO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ENC_PGO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_DIR_Pin */
  GPIO_InitStruct.Pin = ENC_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ENC_DIR_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static bool Steer_ENC_I2C_Init(void) {
	uint8_t buf[3];
	buf[0] = AS5600L_CONFIG_CONF;
	buf[1] = (conf_val >> 8) & 0xFF;
	buf[2] = conf_val & 0xFF;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 3, HAL_MAX_DELAY) != HAL_OK ) {
		/* Error*/
	}

	memset(buf, 0, 3 * sizeof(*buf));

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
		/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	uint16_t recieved_config = (uint16_t)(((uint16_t)buf[1] & 0x3F) << 8 | buf[2]);
	bool proper_init = 0;

	proper_init |= (conf_pm_mask | recieved_config) == (conf_pm_mask | conf_val);
	proper_init |= (conf_hyst_mask | recieved_config) == (conf_hyst_mask | conf_val);
	proper_init |= (conf_outs_mask | recieved_config) == (conf_outs_mask | conf_val);
	proper_init |= (conf_pwmf_mask | recieved_config) == (conf_pwmf_mask | conf_val);
	proper_init |= (conf_sf_mask | recieved_config) == (conf_sf_mask | conf_val);
	proper_init |= (conf_fth_mask | recieved_config) == 	(conf_fth_mask | conf_val);
	proper_init |= (conf_wd_mask | recieved_config) == (conf_wd_mask | conf_val);

	return proper_init;
}

static void Steer_ENC_I2C_Read(void) {
	uint8_t buf[3];

	memset(buf, 0, 3 * sizeof(*buf));

	buf[0] = AS5600L_OUT_ANG;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	i2c_data.angle = (uint16_t)(((uint16_t)buf[1] & 0x0F) << 8 | buf[2]);
}

static void Steer_ENC_I2C_Full_Read(void) {
	// Number of bytes to fill in i2c_data plus 6 pointer changes
	uint8_t buf[6 + sizeof(i2c_data)];

	memset(buf, 0, (6 + sizeof(i2c_data)) * sizeof(*buf));

	// Make use of auto-pointer incrementing
	buf[0] = AS5600L_CONFIG_ZMCO;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[1], 9, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Make use of auto-pointer incrementing
	buf[10] = AS5600L_CONFIG_I2CADDR;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[11], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Doesn't make use of pointer incrementing due to special register
	buf[13] = AS5600L_OUT_ANG_RAW;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[14], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Doesn't make use of pointer incrementing due to special register
	buf[16] = AS5600L_OUT_ANG;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[17], 2, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	// Make use of auto-pointer incrementing
	buf[19] = AS5600L_STATUS_STATUS;

	if ( HAL_I2C_Master_Transmit(&hi2c3, conf_i2c_addr, buf, 1, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error*/
	}
	else {
		if ( HAL_I2C_Master_Receive(&hi2c3, conf_i2c_addr, &buf[11], 4, HAL_MAX_DELAY) != HAL_OK ) {
			/* Error */
		}
	}

	i2c_data.zmco = 		(uint8_t)	(buf[1] & 0x3);
	i2c_data.zpos = 		(uint16_t)	(((uint16_t)buf[2] & 0x0F) << 8 | buf[3]);
	i2c_data.mpos = 		(uint16_t)	(((uint16_t)buf[4] & 0x0F) << 8 | buf[5]);
	i2c_data.mang = 		(uint16_t) 	(((uint16_t)buf[6] & 0x0F) << 8 | buf[7]);
	i2c_data.conf = 		(uint16_t)	(((uint16_t)buf[8] & 0x3F) << 8 | buf[9]);
	i2c_data.i2c_addr = 	(uint8_t)	(buf[11] & 0xFE);
	i2c_data.i2c_updt = 	(uint8_t)	(buf[12] & 0xFE);
	i2c_data.raw_angle = 	(uint16_t)	(((uint16_t)buf[14] & 0x0F) << 8 | buf[15]);
	i2c_data.angle = 		(uint16_t)	(((uint16_t)buf[17] & 0x0F) << 8 | buf[18]);
	i2c_data.status =		(uint8_t)	(buf[20] & 0xFE);
	i2c_data.agc =			(uint8_t)	(buf[21] & 0xFE);
	i2c_data.magnitude = 	(uint16_t)	(((uint16_t)buf[23] & 0x0F) << 8 | buf[24]);
}

static void Steer_ENC_CAN_Message(uint8_t *canTx) {
	memset(canTx, 0, 8 * sizeof(*canTx));

	can_data.flags = (uint16_t)(i2c_data.status | status_mag_mask) >> 3;
	can_data.angle = i2c_data.angle;

	memcpy(canTx, &can_data, sizeof(can_data));
}

static void Steer_ENC_CAN_Transmit(void) {
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;

	TxHeader.DLC = 8; // Data length
	TxHeader.IDE = CAN_ID_STD; // Standard ID
	TxHeader.RTR = CAN_RTR_DATA; // Data frame
	TxHeader.StdId = FEB_CAN_ID_Steer_Wheel_ENC; // CAN ID
	TxHeader.ExtId = 0; // Not used with standard ID

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, canTx, &TxMailbox) != HAL_OK) {
		// Transmission request error
		Error_Handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if ( htim == &htim6 ) {
		Steer_ENC_I2C_Read();
		Steer_ENC_CAN_Message(canTx);
		Steer_ENC_CAN_Transmit();
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
