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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void SendRsp( char *str);
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
#define M1_TX_Pin GPIO_PIN_4
#define M1_TX_GPIO_Port GPIOC
#define M1_RX_Pin GPIO_PIN_5
#define M1_RX_GPIO_Port GPIOC
#define M1_DIS_Pin GPIO_PIN_11
#define M1_DIS_GPIO_Port GPIOA
#define M2_DIS_Pin GPIO_PIN_12
#define M2_DIS_GPIO_Port GPIOA
#define M_RX_Pin GPIO_PIN_15
#define M_RX_GPIO_Port GPIOA
#define M_TX_Pin GPIO_PIN_3
#define M_TX_GPIO_Port GPIOB
#define FT_RX_Pin GPIO_PIN_8
#define FT_RX_GPIO_Port GPIOB
#define FT_TX_Pin GPIO_PIN_9
#define FT_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
