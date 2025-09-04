/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  *
  *SD CARD PINOUTS
  *SCK - PA5
  *MISO - PA6
  *MOSI - PA7
  *CS - PC4
  *CAN TX - PA12
  *CAN RX - PA11
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "FEB_CircularBuffer.h"
#include "FEB_XBee.h"
#include "FEB_CAN.h"
#include "FEB_CAN_Heartbeat.h"
#include "TPS2482.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAIN_DEBUG_ENABLED 1

/* Define XBEE_UART_MODE to 1 to enable UART passthrough mode */
/* When enabled: XBee switches to UART mode for XCTU configuration */
/* When disabled (0): Normal data logging and transmission mode */
#define XBEE_UART_MODE 1

#define XBEE_ATTN_PORT GPIOC
#define XBEE_ATTN_PIN  GPIO_PIN_12
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


circBuffer sdBuffer;
circBuffer xbeeBuffer;
bool xbeeReady = true;
static bool xbee_uart_mode_active = false; /* Track if XBee is in UART mode */

static TPS2482_Configuration tps2482_configurations[1];
uint8_t tps2482_i2c_addresses[1];
static uint16_t tps2482_ids[1];

static void FEB_Variable_Init(void) {
	tps2482_i2c_addresses[0] = TPS2482_I2C_ADDR(TPS2482_I2C_ADDR_GND, TPS2482_I2C_ADDR_GND);
	tps2482_configurations[0].config = TPS2482_CONFIG_DEFAULT;
	tps2482_configurations[0].mask = TPS2482_MASK_SOL;
	tps2482_configurations[0].cal = TPS2482_CAL_EQ(TPS2482_CURRENT_LSB_EQ((double)(5)),(double)(.012));
}

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
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // Initialize SD card once
  FEB_circBuf_sdcard_init();
  
  // Initialize buffer structures
  FEB_circBuf_init(&sdBuffer);
  FEB_circBuf_init(&xbeeBuffer);
  
  // Initialize XBEE with proper startup sequence
  HAL_UART_Transmit(&huart2, (uint8_t*)"Initializing XBEE...\r\n", 22, 100);
  
  /* Configure XBee control pins for proper operation */
  HAL_GPIO_WritePin(GPIOC, XB_ON_Pin, GPIO_PIN_SET);     /* ON pin HIGH to power XBee */
  HAL_GPIO_WritePin(GPIOC, XB_DTR_Pin, GPIO_PIN_RESET);  /* DTR LOW for normal operation */
  HAL_GPIO_WritePin(GPIOC, XB_NRTS_Pin, GPIO_PIN_RESET); /* RTS LOW (active) */
  HAL_GPIO_WritePin(GPIOB, XB_CTS_Pin, GPIO_PIN_RESET);  /* CTS LOW to enable transmission */
  
  /* Reset XBee with proper timing */
  HAL_GPIO_WritePin(GPIOB, XB_RST_Pin, GPIO_PIN_RESET);  /* Assert reset */
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOB, XB_RST_Pin, GPIO_PIN_SET);    /* Release reset */
  HAL_Delay(1000); /* Wait for XBee to fully boot and initialize */
  
  HAL_UART_Transmit(&huart2, (uint8_t*)"XBEE startup complete\r\n", 23, 100);
  
  /* Check startup mode configuration */
#if XBEE_UART_MODE
  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
  HAL_UART_Transmit(&huart2, (uint8_t*)"=================================================\r\n", 51, 100);
  HAL_UART_Transmit(&huart2, (uint8_t*)"=== XBEE MODE SWITCH: SPI -> UART ===\r\n", 40, 100);
  HAL_UART_Transmit(&huart2, (uint8_t*)"=================================================\r\n", 51, 100);
  
  /* Attempt to switch XBee from SPI to UART mode */
  HAL_StatusTypeDef uart_switch_status = FEB_xbee_switch_to_uart_mode();
  
  if (uart_switch_status == HAL_OK) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"=================================================\r\n", 51, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"          XBEE UART SWITCH COMPLETE!\r\n", 39, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"=================================================\r\n", 51, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"NEXT STEPS:\r\n", 13, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"1. Power cycle the XBee module\r\n", 32, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"2. Disconnect from this board\r\n", 31, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"3. Connect XBee to XCTU via USB adapter\r\n", 42, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"4. Configure at 9600 baud\r\n", 27, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"*** SYSTEM HALTED - NO FURTHER OPERATIONS ***\r\n", 48, 100);
  } else {
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"!!! ERROR: UART MODE SWITCH FAILED !!!\r\n", 41, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"The XBee may already be in UART mode or\r\n", 42, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"there was a communication error.\r\n", 34, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"Try power cycling and checking connections.\r\n", 46, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"*** SYSTEM HALTED - NO FURTHER OPERATIONS ***\r\n", 48, 100);
  }
  
  /* Set flags to prevent ALL operations */
  xbeeReady = false;
  xbee_uart_mode_active = true;
  
  /* Infinite loop - system is done, no further operations */
  while (1) {
    HAL_Delay(1000);
    /* Do nothing - system is halted after mode switch */
  }
  
#else
  HAL_UART_Transmit(&huart2, (uint8_t*)"=== XBEE STARTUP: SPI MODE ===\r\n", 33, 100);
  
  // Test XBEE SPI communication with full config display
  FEB_xbee_spi_test();
  HAL_Delay(500);
  
  // Run simple transmission test
  FEB_xbee_simple_test();
  HAL_Delay(500);
  
  FEB_xbee_uart_fallback_test();
#endif
  
  /* Initialize CAN and other peripherals only in normal mode */
#if !XBEE_UART_MODE
  FEB_CAN_Init();
  FEB_CAN_HEARTBEAT_Init();
  FEB_Variable_Init();
  bool tps2482_init_res[1];
  TPS2482_Init(&hi2c1, tps2482_i2c_addresses, tps2482_configurations, tps2482_ids, tps2482_init_res, 1);
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
// uint32_t loop_counter = 0;  // Unused - commented out

while (1)
{
#if XBEE_UART_MODE
    /* This code should never execute - system halts after mode switch */
    /* If we somehow get here, just do nothing */
    HAL_Delay(1000);
#else
    /* Normal operation mode - data logging and transmission */
    
    // --- Check ATTn pin (active LOW) ---
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_RESET) {
#if MAIN_DEBUG_ENABLED
        HAL_UART_Transmit(&huart2, (uint8_t*)"ATTn LOW\r\n", 11, 100);
#endif

        uint8_t xbee_receive_status = FEB_xbee_receive_status();
        
#if MAIN_DEBUG_ENABLED
        char status_msg[50];
        snprintf(status_msg, sizeof(status_msg), "XBEE status: 0x%02X\r\n", xbee_receive_status);
        HAL_UART_Transmit(&huart2, (uint8_t*)status_msg, strlen(status_msg), 100);
#endif

        if (xbee_receive_status == 0x01 || xbee_receive_status == 0x00) { /* 0x00 = success, 0x01 = no ACK but sent */
#if MAIN_DEBUG_ENABLED
            HAL_UART_Transmit(&huart2, (uint8_t*)"Setting xbeeReady=true\r\n", 25, 100);
#endif
            xbeeReady = true;
        } else {
#if MAIN_DEBUG_ENABLED
            HAL_UART_Transmit(&huart2, (uint8_t*)"Setting xbeeReady=false\r\n", 26, 100);
#endif
            xbeeReady = false;
        }

    } else {
        /* ATTn pin is HIGH - force transmission for testing */
        static uint32_t force_counter = 0;
        force_counter++;
        if (force_counter % 1000 == 0) { /* Every 1000 loops */
#if MAIN_DEBUG_ENABLED
            HAL_UART_Transmit(&huart2, (uint8_t*)"ATTn HIGH - forcing XBEE ready\r\n", 33, 100);
#endif
            xbeeReady = true;
        }
    }

    // --- Transmit if xbeeReady is set and not in UART mode ---
    if (xbeeReady == true && !xbee_uart_mode_active) {
#if MAIN_DEBUG_ENABLED
        HAL_UART_Transmit(&huart2, (uint8_t*)"xbeeReady=true, transmitting\r\n", 31, 100);
#endif

        /* Check if buffer has data before transmitting */
        if (xbeeBuffer.count == 0) {
            /* Add test data if buffer is empty */
            uint8_t test_data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
            FEB_circBuf_addOrReplace(&xbeeBuffer, 0x123, test_data);
        }
        
        HAL_StatusTypeDef xbee_status = FEB_xbee_transmit_can_data(&xbeeBuffer);
        if (xbee_status == HAL_OK) {
#if MAIN_DEBUG_ENABLED
            HAL_UART_Transmit(&huart2, (uint8_t*)"CAN data TX success\r\n", 22, 100);
#endif
            xbeeReady = false; /* Only clear flag on successful transmission */
        } else {
#if MAIN_DEBUG_ENABLED
            HAL_UART_Transmit(&huart2, (uint8_t*)"CAN data TX failed\r\n", 21, 100);
#endif
            /* Keep xbeeReady true to retry on next iteration */
            static uint32_t last_error_log = 0;
            uint32_t now = HAL_GetTick();
            if (now - last_error_log > 2000) { /* Throttle error messages */
                last_error_log = now;
            }
        }
    }

    // Write to SD Card
    FEB_circBuf_read(&sdBuffer);
#endif /* XBEE_UART_MODE */
}




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

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
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  hi2c3.Init.OwnAddress1 = 0;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, XB_NRTS_Pin|XB_DTR_Pin|XB_ON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(mSD_CS_GPIO_Port, mSD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, XB_CTS_Pin|XB_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(XB_CS_GPIO_Port, XB_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : XB_NRTS_Pin XB_DTR_Pin XB_ON_Pin */
  GPIO_InitStruct.Pin = XB_NRTS_Pin|XB_DTR_Pin|XB_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : mSD_CS_Pin XB_CS_Pin */
  GPIO_InitStruct.Pin = mSD_CS_Pin|XB_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PG_Pin */
  GPIO_InitStruct.Pin = PG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : XB_CTS_Pin XB_RST_Pin */
  GPIO_InitStruct.Pin = XB_CTS_Pin|XB_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : XB_ATTN_Pin */
  GPIO_InitStruct.Pin = XB_ATTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(XB_ATTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TPS_PG_Pin TPS_ALERT_Pin */
  GPIO_InitStruct.Pin = TPS_PG_Pin|TPS_ALERT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
