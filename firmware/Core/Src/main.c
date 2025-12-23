/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debugLog.h"
#include "i2c_bitbang.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MPU_ADDR   				0x68
#define MPU_REG_ACCEL_X        	0x3b
#define MPU_REG_ACCEL_Y        	0x3d
#define LightSwitch_Pin        	GPIO_PIN_14
#define LightSwitch_GPIO_Port  	GPIOC


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int16_t xAccelArray[8];
int16_t yAccelArray[8];
uint8_t accelArray_pos=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int16_t getMedian(uint8_t arraySel){
	int16_t sum=0;
	for(uint8_t i=0;i<8;i++){
		if (arraySel == 0)
			sum += xAccelArray[i];
		else
			sum += yAccelArray[i];

	}
	return sum/8;
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t counter = 0, onCnt = 0, moveDetectCnt = 0;
  i2cWrite2bytes(MPU_ADDR, 0x6b, 0x00); // dummy write after power up
  i2cWrite2bytes(MPU_ADDR, 0x6b, 0x00); // PWR_MGMT_1 register - wake up
  i2cWrite2bytes(MPU_ADDR, 0x19, 0xff); // sample rate divider - highest divider
  i2cWrite2bytes(MPU_ADDR, 0x1c, 0x18); // Accel config register - scale to +/-16g

  //#define USE_DEBUG_LOG // debug log pin and light switch uses the same pin

  while (1)
  {
	  if (onCnt>0){
		  onCnt-=2;
	  }

	  i2cWrite2bytes(MPU_ADDR, 0x6b, 0x00); // PWR_MGMT_1 register - wake up
	  i2cWrite1byte(MPU_ADDR, MPU_REG_ACCEL_X);
	  int16_t accel_x = (int16_t)i2cRead2bytes(MPU_ADDR);
	  i2cWrite1byte(MPU_ADDR, MPU_REG_ACCEL_Y);
	  int16_t accel_y = (int16_t)i2cRead2bytes(MPU_ADDR);
	  i2cWrite2bytes(MPU_ADDR, 0x6b, 0x40); // PWR_MGMT_1 register - sleep

	  int16_t xDiff = getMedian(0) - accel_x;
	  int16_t yDiff = getMedian(1) - accel_y;

	  if (xDiff > 200 || xDiff < -200 || yDiff > 200 || yDiff < -200){ // movement detected
		  moveDetectCnt++;
		  if (moveDetectCnt>1)
			  onCnt=60; // set light on time
	  }
	  else
		  moveDetectCnt = 0;

	  // add to median-calc-array
	  xAccelArray[accelArray_pos] = accel_x;
	  yAccelArray[accelArray_pos] = accel_y;
	  accelArray_pos++;
	  if (accelArray_pos>7)
		  accelArray_pos=0;

	  // log
	  #ifdef USE_DEBUG_LOG
	  counter++;
	  sendDebugByte(counter>>8);
	  sendDebugByte(counter);
	  sendDebugByte(accel_x >> 8);
	  sendDebugByte(accel_x & 0xff);
	  sendDebugByte(accel_y >> 8);
	  sendDebugByte(accel_y & 0xff);
	  sendDebugByte(onCnt>>8);
	  sendDebugByte(onCnt);
	  #else
	  if (onCnt > 0){
		  HAL_GPIO_WritePin(LightSwitch_GPIO_Port, LightSwitch_Pin, GPIO_PIN_SET); // switch on
	  }
	  else{
		  HAL_GPIO_WritePin(LightSwitch_GPIO_Port, LightSwitch_Pin, GPIO_PIN_RESET); // switch off
	  }
	  #endif

	  HAL_Delay(2000);
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

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV128;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  HAL_SYSCFG_SetPinBinding(HAL_BIND_SO8_PIN1_PC14|HAL_BIND_SO8_PIN5_PA8);

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
