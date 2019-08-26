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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"
#include "system_watchdog.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Defines for external led signaling
#define USER_ALARM_LED GPIOB
#define NO_ALARM_LED GPIO_PIN_1
#define AERIAL_ALARM_LED GPIO_PIN_13
#define TERRESTRIAL_ALARM_LED GPIO_PIN_14
#define SNAKE_ALARM_LED GPIO_PIN_15
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// Main global variables
bool system_ok = false;					// Global variable indicating the status of the system, if true the system is properly
										// working, otherwise false
bool flash = true;						// when true the led matrix flashes
uint16_t n = 0;							// inner counter for system status watchdog
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void user_led();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Miscellaneous global variables definition (extern variables)
bool leds_on = false;
char current_alarm = 'n';

// ADC DMA buffer
uint32_t dma_adc_buffer[5];

// Audio related global variables definition (extern variables)
uint16_t count = 0;
bool microphones_booting = true;

// Motor related global variables definition (extern variables)
bool motor_booting = true;
bool end_of_run = true;

// GSM module related global variables definition
bool GSM_new_response = false;
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM9_Init();
  MX_TIM1_Init();
  MX_TIM11_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // -------------------------------------------- SYSTEM INITIALIZATION -------------------------------------------------------- //
  system_ok = system_init();
  // Once the system is started enable watchdog interrupts
  // Enable system integrity interrupt
  HAL_TIM_Base_Start_IT(&htim9);					// approx. every 60sec system watchdog checks if the system is ok
  // Enable system watchdog timeout
  HAL_TIM_Base_Start_IT(&htim11);					// alarm detection procedure takes approx. 0.5sec, so a good tradeoff is a
  	  	  	  	  	  	  	  	  	  	  	  	  	// watchdog 10 times faster that controls the current alarm detected
  // ------------------------------------------- INITIALIZATION COMPLETED ------------------------------------------------------ //
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // ------------------------------------------------- SYSTEM STATUS ------------------------------------------------------- //
	  if(!system_ok) {									// if any device is not working shutdown all
		  HAL_UART_Transmit(&huart2, (uint8_t*)"Something went wrong...\r\n", sizeof("Something went wrong...\r\n"), HAL_MAX_DELAY);
		  HAL_Delay(3000);
		  system_shutdown();
	  }
	  user_led();
	  //HAL_UART_Transmit(&huart2, (uint8_t*)"Tutto ok\r\n", sizeof("Tutto ok\r\n"), HAL_MAX_DELAY);
									// external user signaling
	  // ----------------------------------------------------------------------------------------------------------------------- //
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
// ----------------------------------------- INTERRUPT EVENTS CALLBACK HANDLING ----------------------------------------------------- //
// Callback function handling completed ADC conversions
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	// Processes recorded audio and boot microphones controlling they are ok
	if(!microphones_booting && !motor_booting)
		record_audio();
	// Controls motor turn during initialization phase
	if(motor_booting) {
		if(dma_adc_buffer[4] < 3000) {
			end_of_run = true;
		}
	}
}

// ADC overrun error callback. If this happens restart ADC and DMA
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	if(hadc->Instance == ADC1)
		HAL_ADC_Start_DMA(&hadc1, dma_adc_buffer, 5);
}

// Callback function handling timers interruptions
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	// Control alarm handling
	if(htim->Instance == TIM11) {
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		if(current_alarm != 'n')
			alarm_timeout();
	}
	// Led matrix flash effect
	if(htim->Instance == TIM9) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		if(flash && leds_on)
			turn_on_led_matrix();
		else
			turn_off_led_matrix();
		flash = !flash;
		// counter to control 120sec watchdog
		n++;
		if(n == 120) {
			system_ok = check_system_status();
			n = 0;
		}
	}
}

// GSM module UART reception trigger
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	GSM_new_response = true;
}

// GSM module UART error handling. If this happens restart IT receive
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	HAL_UART_Receive_IT(&huart1, GSM_response, GSM_new_response_size);
	GSM_new_response = true;
}
// --------------------------------------------------------------------------------------------------------------------------------- //

/** @brief Lights up a led of different color to indicate the system state
 *  @param  none
 *  @return none
 */
void user_led() {
	// Depending on the current alarm lights up a different led
	switch(current_alarm) {
	// Set GPIO with atomic instructions
	case 'n':
		USER_ALARM_LED->BSRR = NO_ALARM_LED;
		USER_ALARM_LED->BSRR = (uint32_t)SNAKE_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)AERIAL_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)TERRESTRIAL_ALARM_LED << 16U;
		break;
	case 's':
		USER_ALARM_LED->BSRR = SNAKE_ALARM_LED;
		USER_ALARM_LED->BSRR = (uint32_t)NO_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)AERIAL_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)TERRESTRIAL_ALARM_LED << 16U;
		break;
	case 'e':
		USER_ALARM_LED->BSRR = AERIAL_ALARM_LED;
		USER_ALARM_LED->BSRR = (uint32_t)NO_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)SNAKE_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)TERRESTRIAL_ALARM_LED << 16U;
		break;
	case 'l':
		USER_ALARM_LED->BSRR = TERRESTRIAL_ALARM_LED;
		USER_ALARM_LED->BSRR = (uint32_t)NO_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)SNAKE_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)AERIAL_ALARM_LED  << 16U;
		break;
	default:
		USER_ALARM_LED->BSRR = NO_ALARM_LED;
		USER_ALARM_LED->BSRR = (uint32_t)SNAKE_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)AERIAL_ALARM_LED << 16U;
		USER_ALARM_LED->BSRR = (uint32_t)TERRESTRIAL_ALARM_LED << 16U;
		break;
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
  //system_shutdown();
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
