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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Keypad4X4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fonts.h"
#include "ssd1306.h"

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for keypadTast */
osThreadId_t keypadTastHandle;
const osThreadAttr_t keypadTast_attributes = {
  .name = "keypadTast",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for motionTask */
osThreadId_t motionTaskHandle;
const osThreadAttr_t motionTask_attributes = {
  .name = "motionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for outputTask */
osThreadId_t outputTaskHandle;
const osThreadAttr_t outputTask_attributes = {
  .name = "outputTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */
extern char key;
char masterkey[6];
int masterlen = 0;
char asterisk[6];
char hold[6];
int holdlen = 0;
int motion = 0;

int alarmgraceperiod = 5; // 5 seconds - should be 60 seconds
char alarmgracestring[3];
int alarmgraceflag = 0; //0 for alarm, 1 for arming grace, 2 for movement detected grace
int wipescreenflag = 0;

int armed;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void StartKeypadTask(void *argument);
void StartMotionSensor(void *argument);
void StartOutputTask(void *argument);

/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE BEGIN 2 */
    SSD1306_Init();
    SSD1306_GotoXY (0,0);
    //SSD1306_Puts ("Voltage:", &Font_11x18, 1);
    SSD1306_Puts ("Lab 4:", &Font_11x18, 1);
    SSD1306_GotoXY (0, 30);
    SSD1306_UpdateScreen();
    HAL_Delay (500);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of keypadTast */
  keypadTastHandle = osThreadNew(StartKeypadTask, NULL, &keypadTast_attributes);

  /* creation of motionTask */
  motionTaskHandle = osThreadNew(StartMotionSensor, NULL, &motionTask_attributes);

  /* creation of outputTask */
  outputTaskHandle = osThreadNew(StartOutputTask, NULL, &outputTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

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
  hi2c1.Init.ClockSpeed = 400000;
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
  huart2.Init.BaudRate = 9600;
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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|KC0_Pin|GPIO_PIN_13|GPIO_PIN_14
                          |KC3_Pin|KC1_Pin|KC2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 KC0_Pin PB13 PB14
                           KC3_Pin KC1_Pin KC2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2|KC0_Pin|GPIO_PIN_13|GPIO_PIN_14
                          |KC3_Pin|KC1_Pin|KC2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : KR1_Pin */
  GPIO_InitStruct.Pin = KR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KR1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KR3_Pin KR2_Pin */
  GPIO_InitStruct.Pin = KR3_Pin|KR2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : KR0_Pin */
  GPIO_InitStruct.Pin = KR0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KR0_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartKeypadTask */
/**
* @brief Function implementing the keypadTast thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeypadTask */
void StartKeypadTask(void *argument)
{
  /* USER CODE BEGIN StartKeypadTask */
  /* Infinite loop */
  for(;;)
  {
	 key = Get_Key();


	 if (armed == 0) {

		 if (key == '*') { // Submitted
			 wipescreenflag = 1;
			if (masterlen >= 4 && masterlen <= 6) {
				armed = 1; // arm the system

				//Allow grace period for user to walk away
				alarmgraceperiod = 5;
				alarmgraceflag = 1;
			} else {
				continue;
			}
		 } else if (masterlen < 6) {
			masterkey[masterlen] = key;
			masterlen++;
		 }

	 } else {

		 if (key == '*') { // Submitted
			 wipescreenflag = 1;
			 int match = 1;
			 if (masterlen != holdlen) {
				 match = 0;
			 } else {
				 for (int i=0;i<masterlen;i++){
					 if (masterkey[i] != hold[i]) {
						 match = 0;
					 }
				 }
			 }

			 if (match == 1) {
				 armed = 0;
				 //Clear master
				 for (int i=0;i<masterlen;i++){
					 masterkey[i] = ' ';
				 }
				 masterlen = 0;
				 //Clear hold
				 for (int i=0;i<holdlen;i++){
					 hold[i] = ' ';
					 asterisk[i] = ' ';
				 }
				 holdlen = 0;

			 } else {
				 //Clear hold
				 for (int i=0;i<holdlen;i++){
					 hold[i] = ' ';
					 asterisk[i] = ' ';
				 }
				 holdlen = 0;
			 }
		 } else if (holdlen < 6) {
			 hold[holdlen] = key;
			 asterisk[holdlen] = '*';
			 holdlen++;
		 }
	 }

	 HAL_Delay(100);


  }
  /* USER CODE END StartKeypadTask */
}

/* USER CODE BEGIN Header_StartMotionSensor */
/**
* @brief Function implementing the motionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotionSensor */
void StartMotionSensor(void *argument)
{
  /* USER CODE BEGIN StartMotionSensor */
  /* Infinite loop */
  for(;;)
  {

	  if (armed == 1) {

		 //Arming grace period
		 if (alarmgraceflag == 1) {
			if (alarmgraceperiod == 0) {
				alarmgraceflag = 0;
			} else {
				HAL_Delay(1000);
				alarmgraceperiod--;
			}

			continue;
		}

		if (alarmgraceflag == 0) {
			motion = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
			if (motion == 1) { // Detected motion
				alarmgraceperiod = 5;
				alarmgraceflag = 2;
			}

		}

		if (alarmgraceflag == 2) {
			//Detection grace period
			if (alarmgraceperiod == 0) {
				//Activate buzzer
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
			} else {
				HAL_Delay(1000);
				alarmgraceperiod--;
			}

			continue;

		}

    }

    //Deactivate buzzer
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);

	HAL_Delay(500);
  }
  /* USER CODE END StartMotionSensor */
}

/* USER CODE BEGIN Header_StartOutputTask */
/**
* @brief Function implementing the outputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOutputTask */
void StartOutputTask(void *argument)
{
  /* USER CODE BEGIN StartOutputTask */
  /* Infinite loop */
  for(;;)
  {

	  if (wipescreenflag == 1) {
		  SSD1306_Clear();
		  wipescreenflag = 0;
	  }

	if (armed == 0) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

		SSD1306_GotoXY(0,0);
		SSD1306_Puts("NOT ARMED", &Font_11x18,1);
		SSD1306_GotoXY(0,30);
		SSD1306_Puts(masterkey, &Font_11x18,1);
		SSD1306_UpdateScreen();

	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

		SSD1306_GotoXY(0,0);
		SSD1306_Puts("ARMED", &Font_11x18,1);
		SSD1306_GotoXY(0,30);
		SSD1306_Puts(asterisk, &Font_11x18,1);
		SSD1306_GotoXY(100,30);
		sprintf(alarmgracestring,"%d", alarmgraceperiod);
		SSD1306_Puts(alarmgracestring, &Font_11x18,1);
		SSD1306_UpdateScreen();

	}

	HAL_Delay(250);


  }
  /* USER CODE END StartOutputTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
