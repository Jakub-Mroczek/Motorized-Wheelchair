/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t power = 0;

uint8_t red_Dflag = 0;
uint8_t red_flag = 0;
uint8_t orange_flag = 0;
uint8_t yellow_flag = 0;
uint8_t green_flag = 0;

uint8_t buff[] = "Starting Up... \r \n";
char line1 []  = {254, 128};
char line2 []  = {254, 192};
uint8_t count1 = 0;
uint8_t toggleFlagForward = 0;
uint8_t toggleFlagBackward = 0;
uint8_t toggleFlagRight = 0;
uint8_t toggleFlagLeft = 0;

uint8_t iADC1 = 0; //Loop counter
uint16_t receiveBuffer1; //ADC receive buffer
uint16_t receiveResult1; //ADC processed result
float CalcVoltagePOT1; //ADC calculated voltage

uint16_t receiveBuffer2; //ADC receive buffer
uint16_t receiveResult2; //ADC processed result
float CalcVoltagePOT2; //ADC calculated voltage

uint16_t receiveBuffer3; //ADC receive buffer
uint16_t receiveResult3; //ADC processed result
float CalcVoltageBAT; //ADC calculated voltage

float ForwardVoltAdj; //Adjusted forward and back voltage
float HorizVoltAdj; //Adjusted side to side voltage (turning)

uint32_t oldtimeL = 0;
uint32_t oldtimeR = 0;
uint32_t newtimeL = 0;
uint32_t newtimeR = 0;
double RPML = 0.0;
double RPMR = 0.0;
uint8_t ML_flag = 0;
uint8_t MR_flag = 0;
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
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim2); //Start the TIM2 module with interrupts enabled
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //Start PWM on CH1
  //HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);

  //Initialize SCLK and CSb (Chip Select bar, where 'bar' means logical 'not' a.k.a. active-low)
    HAL_GPIO_WritePin(SCLK1_GPIO_Port, SCLK1_Pin, GPIO_PIN_RESET); //SCLK1 = 0
    HAL_GPIO_WritePin(CSB1_GPIO_Port, CSB1_Pin, GPIO_PIN_SET); //CBb1 = 1

    HAL_GPIO_WritePin(SCLK2_GPIO_Port, SCLK2_Pin, GPIO_PIN_RESET); //SCLK2 = 0
    HAL_GPIO_WritePin(CSB2_GPIO_Port, CSB2_Pin, GPIO_PIN_SET); //CBb2 = 1

    HAL_GPIO_WritePin(SCLK3_GPIO_Port, SCLK3_Pin, GPIO_PIN_RESET); //SCLK3 = 0
    HAL_GPIO_WritePin(CSB3_GPIO_Port, CSB3_Pin, GPIO_PIN_SET); //CBb3 = 1

    oldtimeL = HAL_GetTick();
    oldtimeR = oldtimeL;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET)
	  {
		  // Set power flag high
		  power = 1;

	  	  HAL_UART_Transmit(&huart6, line2, sizeof(line2), HAL_MAX_DELAY);
	  	  sprintf((char*)buff, "RPM MR: %.3f\r\n", RPMR);
		  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  	  HAL_UART_Transmit(&huart6, line1, sizeof(line1), HAL_MAX_DELAY);
	  	  sprintf((char*)buff, "RPM ML: %.3f\r\n", RPML);
	  	  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  HAL_GPIO_WritePin(CSB1_GPIO_Port, CSB1_Pin, GPIO_PIN_RESET); //CBb1 = 0
	  HAL_GPIO_WritePin(CSB2_GPIO_Port, CSB2_Pin, GPIO_PIN_RESET); //CBb2 = 0
	  HAL_GPIO_WritePin(CSB3_GPIO_Port, CSB3_Pin, GPIO_PIN_RESET); //CBb3 = 0

	  	  receiveBuffer1 = 0;
	  	  receiveBuffer2 = 0;
	  	  receiveBuffer3 = 0;

	  	  for (iADC1 = 0; iADC1 < 16; iADC1++)
	  	  {
	  		  receiveBuffer1 = receiveBuffer1 << 1; //Make room for newly received bit by left shifting
	  		  HAL_GPIO_WritePin(SCLK1_GPIO_Port, SCLK1_Pin, GPIO_PIN_SET); //SCLK1 = 1

	  		  if (HAL_GPIO_ReadPin(SDA1_GPIO_Port, SDA1_Pin) == GPIO_PIN_SET) //Is it a '1'?
	  		  {
	  			  receiveBuffer1 = receiveBuffer1 + 1;
	  		  }
	  		  HAL_GPIO_WritePin(SCLK1_GPIO_Port, SCLK1_Pin, GPIO_PIN_RESET); //SCLK1 = 0
	  	  }
  	  	  HAL_GPIO_WritePin(CSB1_GPIO_Port, CSB1_Pin, GPIO_PIN_SET); //CBb1 = 1
  	  	  receiveResult1 = (receiveBuffer1 >> 3) & 0x03FF; //Discard non-useful data bits keeping only 10-bits (see datasheet)
  	  	  CalcVoltagePOT1 = (float)receiveResult1/1023*3.3; //Max value is 1023 which represents 3.3V
  	  	  HAL_Delay(10);
	  	  for (iADC1 = 0; iADC1 < 16; iADC1++)
	  	  {
	  		  receiveBuffer2 = receiveBuffer2 << 1; //Make room for newly received bit by left shifting
	  		  HAL_GPIO_WritePin(SCLK2_GPIO_Port, SCLK2_Pin, GPIO_PIN_SET); //SCLK2 = 1

	  		  if (HAL_GPIO_ReadPin(SDA2_GPIO_Port, SDA2_Pin) == GPIO_PIN_SET) //Is it a '1'?
			  {
				  receiveBuffer2 = receiveBuffer2 + 1;
			  }

	  		  HAL_GPIO_WritePin(SCLK2_GPIO_Port, SCLK2_Pin, GPIO_PIN_RESET); //SCLK2 = 0
	  	  }
	  	  HAL_GPIO_WritePin(CSB2_GPIO_Port, CSB2_Pin, GPIO_PIN_SET); //CBb2 = 1
	  	  receiveResult2 = (receiveBuffer2 >> 3) & 0x03FF; //Discard non-useful data bits keeping only 10-bits (see datasheet)
	  	  CalcVoltagePOT2 = (float)receiveResult2/1023*3.3; //Max value is 1023 which represents 3.3V
	  	  HAL_Delay(10);
	  	  for (iADC1 = 0; iADC1 < 16; iADC1++)
	  	  {
	  		  receiveBuffer3 = receiveBuffer3 << 1; //Make room for newly received bit by left shifting
	  		  HAL_GPIO_WritePin(SCLK3_GPIO_Port, SCLK3_Pin, GPIO_PIN_SET); //SCLK3 = 1

	  		  if (HAL_GPIO_ReadPin(SDA3_GPIO_Port, SDA3_Pin) == GPIO_PIN_SET) //Is it a '1'?
			  {
				  receiveBuffer3 = receiveBuffer3 + 1;
			  }

	  		  HAL_GPIO_WritePin(SCLK3_GPIO_Port, SCLK3_Pin, GPIO_PIN_RESET); //SCLK3 = 0
	  	  }
	  	  HAL_GPIO_WritePin(CSB3_GPIO_Port, CSB3_Pin, GPIO_PIN_SET); //CBb3 = 1
	  	  receiveResult3 = (receiveBuffer3 >> 3) & 0x03FF; //Discard non-useful data bits keeping only 10-bits (see datasheet)
	  	  CalcVoltageBAT =  (float)receiveResult3/1023*3.3; //Max value is 1023 which represents 3.3V
	  	  HAL_Delay(10);
	  	  ForwardVoltAdj = (CalcVoltagePOT1-1.65)*2;
	  	  HorizVoltAdj = (CalcVoltagePOT2-1.65)*2;

	  	  if(fabs((ForwardVoltAdj) < 0.05) && ML_flag != 1 && MR_flag != 1)
	  	  {
	  		  uint32_t now = HAL_GetTick();

	  		  if(HorizVoltAdj < 0.1)
	  		  {
	  			  double deltaTL = (now - oldtimeL)/1000.0;
				  if(deltaTL > 2.3)
				  {
					  RPML = 0;
				  }
	  		  }
	  		  if(HorizVoltAdj > -0.1)
	  		  {
		  		  double deltaTR = (now - oldtimeR)/1000.0;
				  if(deltaTR > 2.3)
				  {
					  RPMR = 0;
				  }
	  		  }
	  	  }

		  if(ML_flag)
		  {
			  double deltaTL = (newtimeL - oldtimeL)/1000.0;
			  RPML = 60.0/deltaTL;
			  if(ForwardVoltAdj < -0.1)
			  {
				  RPML = RPML * -1;
			  }
			  oldtimeL = newtimeL;
			  ML_flag = 0;
		  }
		  if(MR_flag)
		  {
			  double deltaTR = (newtimeR - oldtimeR)/1000.0;
			  RPMR = 60.0/deltaTR;
			  if(ForwardVoltAdj < -0.1)
			  {
				 RPMR = RPMR * -1;
			  }
			  oldtimeR = newtimeR;
			  MR_flag = 0;
		  }

	  	  HAL_UART_Transmit(&huart6, line2, sizeof(line2), HAL_MAX_DELAY);
//	  	  sprintf((char*)buff, "BAT V: %.3f\r\n", CalcVoltageBAT);
	  	  sprintf((char*)buff, "RPM MR: %.3f\r\n", RPMR);
		  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  	  HAL_UART_Transmit(&huart6, line1, sizeof(line1), HAL_MAX_DELAY);
	  	  sprintf((char*)buff, "RPM ML: %.3f\r\n", RPML);
	  	  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  	  if((CalcVoltageBAT > 2.95) && (green_flag == 0))
	  	  {
	  		red_Dflag = 0;
	  		red_flag = 0;
			orange_flag = 0;
			yellow_flag = 0;
			green_flag = 1;
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	  	  }
	  	  else if((CalcVoltageBAT <= 2.95) && (CalcVoltageBAT > 2.62) && (yellow_flag == 0))
	  	  {
	  		red_Dflag = 0;
			red_flag = 0;
			orange_flag = 0;
			yellow_flag = 1;
			green_flag = 0;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  }
	  	  else if((CalcVoltageBAT <= 2.62) && (CalcVoltageBAT > 1.96) && (orange_flag == 0))
	  	  {
	  		red_Dflag = 0;
	  		red_flag = 0;
	  		orange_flag = 1;
	  		yellow_flag = 0;
	  		green_flag = 0;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  }
	  	  else if((CalcVoltageBAT <= 1.96) && (CalcVoltageBAT >= 0) && (red_flag == 0))
	  	  {
	  		red_Dflag = 0;
	  		red_flag = 1;
	  		orange_flag = 0;
	  		yellow_flag = 0;
	  		green_flag = 0;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  }

	  	  if(red_flag == 1)
	  	  {
	  		  int duty_cycle = 500;
			  if ((red_Dflag == 0) && (__HAL_TIM_GetCounter(&htim2) > duty_cycle))
			  {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
				  red_Dflag = 1;
			  }
			  else if ((red_Dflag == 1) && (__HAL_TIM_GetCounter(&htim2) <= duty_cycle))
			  {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
				  red_Dflag = 0;
			  }
	  	  }

		  if((fabs(HorizVoltAdj) < 0.04) && (fabs(ForwardVoltAdj) < 0.04))
		  {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
		  }
		  else if(fabs(HorizVoltAdj) < 0.04)
	  	  {
	  		 float duty_cycle = 999.0-(fabs(ForwardVoltAdj)/3.3)*999.0;
//	  		 HAL_UART_Transmit(&huart6, line2, sizeof(line2), HAL_MAX_DELAY);
//	  		 sprintf((char*)buff, "Duty: %.3f\r\n", duty_cycle);
//	  		 HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);
	  		 if(fabs(ForwardVoltAdj) < 0.04)
	  		 {
	  			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	  		 }
	  		 else if(ForwardVoltAdj > 0)
	  		 {
	  			if ((toggleFlagForward == 0) && (__HAL_TIM_GetCounter(&htim2) > duty_cycle))
	  			{
	  				toggleFlagForward = 1; //Using a flag to avoid writing the same state the to output over and over
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	  				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	  			}
	  			if ((toggleFlagForward == 1) && (__HAL_TIM_GetCounter(&htim2) <= duty_cycle))
	  			{
	  				toggleFlagForward = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	  				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	  			}
	  		 }
	  		 else if(ForwardVoltAdj < 0)
	  		 {
		  		if ((toggleFlagBackward == 0) && (__HAL_TIM_GetCounter(&htim2) > duty_cycle))
		  		{
		  			toggleFlagBackward = 1; //Using a flag to avoid writing the same state the to output over and over
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
		  		}
		  		if ((toggleFlagBackward == 1) && (__HAL_TIM_GetCounter(&htim2) <= duty_cycle))
		  		{
		  			toggleFlagBackward = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
		  		}
	  		 }

	  	  }
	  	  else if(fabs(ForwardVoltAdj) < 0.04)
	  	  {
			 float duty_cycle = 999.0-(fabs(HorizVoltAdj)/3.3)*999.0;
//	  		 HAL_UART_Transmit(&huart6, line1, sizeof(line1), HAL_MAX_DELAY);
//	  		 sprintf((char*)buff, "Duty: %.3f\r\n", duty_cycle);
//	  		 HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);
			 if(fabs(HorizVoltAdj) < 0.04)
			 {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
			 }
			 else if(HorizVoltAdj > 0)
			 {
				if ((toggleFlagRight == 0) && (__HAL_TIM_GetCounter(&htim2) > duty_cycle))
				{
					toggleFlagRight = 1; //Using a flag to avoid writing the same state the to output over and over
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				}
				if ((toggleFlagRight == 1) && (__HAL_TIM_GetCounter(&htim2) <= duty_cycle))
				{
					toggleFlagRight = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				}
			 }
			 else if(HorizVoltAdj < 0)
			 {
				if ((toggleFlagLeft == 0) && (__HAL_TIM_GetCounter(&htim2) > duty_cycle))
				{
					toggleFlagLeft = 1; //Using a flag to avoid writing the same state the to output over and over
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				}
				if ((toggleFlagLeft == 1) && (__HAL_TIM_GetCounter(&htim2) <= duty_cycle))
				{
					toggleFlagLeft = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				}
			 }
	  	  }
	  	  else
	  	  {

	  	  }
	  }
	  else
	  {
		  power = 0;
		  RPML = 0.0;
		  RPMR = 0.0;

	  	  HAL_UART_Transmit(&huart6, line2, sizeof(line2), HAL_MAX_DELAY);
	  	  sprintf((char*)buff, "OFF           \r\n");
		  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  	  HAL_UART_Transmit(&huart6, line1, sizeof(line1), HAL_MAX_DELAY);
	  	  sprintf((char*)buff, "POWER         \r\n", RPML);
	  	  HAL_UART_Transmit(&huart6, buff, strlen((char*)buff), HAL_MAX_DELAY);

	  	  // Turn off LEDs
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  red_Dflag = 0;
	  	  red_flag = 0;
	  	  orange_flag = 0;
	  	  yellow_flag = 0;
	  	  green_flag = 0;

	  	  // Turn off Motors
	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	  }

//		  HAL_Delay(20);
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
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1600-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|SCLK1_Pin|SCLK3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CSB1_Pin|CSB3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CSB2_GPIO_Port, CSB2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SCLK2_GPIO_Port, SCLK2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC1 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 PC9 PC10
                           PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 CSB1_Pin SCLK1_Pin
                           CSB3_Pin SCLK3_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|CSB1_Pin|SCLK1_Pin
                          |CSB3_Pin|SCLK3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA1_Pin SDA3_Pin */
  GPIO_InitStruct.Pin = SDA1_Pin|SDA3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CSB2_Pin SCLK2_Pin */
  GPIO_InitStruct.Pin = CSB2_Pin|SCLK2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SDA2_Pin */
  GPIO_InitStruct.Pin = SDA2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SDA2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_2)
    {
    	newtimeL = HAL_GetTick();
    	ML_flag = 1;
    }
    else if(GPIO_Pin == GPIO_PIN_7)
    {
    	newtimeR = HAL_GetTick();
    	MR_flag = 1;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
