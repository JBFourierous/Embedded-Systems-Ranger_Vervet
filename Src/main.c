/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "camera.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void transmit_image(uint8_t image_buffer[][2*COLS]);
void startNewFrame(uint8_t pixelFormat);
void endOfLine();
void sendBlankFrame();
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
  volatile int res = 0, pixels = 0;
  uint8_t image_buffer[ROWS][2*COLS];
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  camera_init(&hi2c1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  //camera_init(&hi2c1);
	  //res = camera_check();
	  pixels = camera_capture_image(image_buffer);
	  //pixels = camera_capture_image2(image_buffer);
	  transmit_image(image_buffer);
	  //sendBlankFrame();
	  //HAL_Delay(5000);
	  //HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_2);
}

/* USER CODE BEGIN 4 */
void transmit_image(uint8_t image_buffer[][2*COLS]) {
	char* tmp = NULL;
	startNewFrame(1);
	for(int i = 0; i < ROWS; i++) {
		for(int j = 0; j < 2*COLS; j++) {
			//sprintf(tmp, "%d\n", image_buffer[i][j]);
			//HAL_UART_Transmit(&huart2, tmp, 1, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, &image_buffer[i][j], 1, HAL_MAX_DELAY);
		}
		endOfLine();
	}
	HAL_UART_Transmit(&huart2, (uint8_t*)"\n", strlen("\n"), HAL_MAX_DELAY);
}

void startNewFrame(uint8_t pixelFormat) {
  volatile uint8_t UDR0 = 0x00;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
  UDR0 = 0x01;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);

  // frame width
  UDR0 = (COLS >> 8) & 0xFF;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
  UDR0 = COLS & 0xFF;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);

  // frame height
  UDR0 = (ROWS >> 8) & 0xFF;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
  UDR0 = ROWS & 0xFF;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);

  // pixel format
  UDR0 = (pixelFormat);
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
}

void endOfLine()   {
  uint8_t UDR0 = 0x00;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
  UDR0 = 0x02;
  HAL_UART_Transmit(&huart2, &UDR0, 1, 10);
}

void sendBlankFrame() {
  uint8_t colorH = (0x07E0 >> 8) & 0xFF;
  uint8_t colorL = 0x07E0 & 0xFF;

  startNewFrame(1);
  for (uint16_t j=0; j<ROWS; j++) {
    for (uint16_t i=0; i<COLS; i++) {
      HAL_UART_Transmit(&huart2, &colorH, 1, HAL_MAX_DELAY);
      HAL_UART_Transmit(&huart2, &colorL, 1, HAL_MAX_DELAY);
    }
    endOfLine();
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
