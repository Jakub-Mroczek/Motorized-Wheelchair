/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CSB1_Pin GPIO_PIN_12
#define CSB1_GPIO_Port GPIOB
#define SDA1_Pin GPIO_PIN_13
#define SDA1_GPIO_Port GPIOB
#define SCLK1_Pin GPIO_PIN_14
#define SCLK1_GPIO_Port GPIOB
#define CSB2_Pin GPIO_PIN_10
#define CSB2_GPIO_Port GPIOA
#define SDA2_Pin GPIO_PIN_11
#define SDA2_GPIO_Port GPIOA
#define SCLK2_Pin GPIO_PIN_12
#define SCLK2_GPIO_Port GPIOA
#define CSB3_Pin GPIO_PIN_5
#define CSB3_GPIO_Port GPIOB
#define SDA3_Pin GPIO_PIN_6
#define SDA3_GPIO_Port GPIOB
#define SCLK3_Pin GPIO_PIN_7
#define SCLK3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
