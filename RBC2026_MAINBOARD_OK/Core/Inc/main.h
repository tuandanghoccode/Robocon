/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#define in5_Pin GPIO_PIN_2
#define in5_GPIO_Port GPIOE
#define in6_Pin GPIO_PIN_3
#define in6_GPIO_Port GPIOE
#define btn1_Pin GPIO_PIN_4
#define btn1_GPIO_Port GPIOE
#define btn2_Pin GPIO_PIN_5
#define btn2_GPIO_Port GPIOE
#define led_pc13_Pin GPIO_PIN_13
#define led_pc13_GPIO_Port GPIOC
#define led_pc14_Pin GPIO_PIN_14
#define led_pc14_GPIO_Port GPIOC
#define led_pc15_Pin GPIO_PIN_15
#define led_pc15_GPIO_Port GPIOC
#define in7_Pin GPIO_PIN_0
#define in7_GPIO_Port GPIOC
#define in8_Pin GPIO_PIN_1
#define in8_GPIO_Port GPIOC
#define in9_Pin GPIO_PIN_2
#define in9_GPIO_Port GPIOC
#define in10_Pin GPIO_PIN_3
#define in10_GPIO_Port GPIOC
#define s0_Pin GPIO_PIN_1
#define s0_GPIO_Port GPIOA
#define s1_Pin GPIO_PIN_2
#define s1_GPIO_Port GPIOA
#define s2_Pin GPIO_PIN_3
#define s2_GPIO_Port GPIOA
#define s3_Pin GPIO_PIN_4
#define s3_GPIO_Port GPIOA
#define o1_Pin GPIO_PIN_1
#define o1_GPIO_Port GPIOB
#define o2_Pin GPIO_PIN_7
#define o2_GPIO_Port GPIOE
#define o3_Pin GPIO_PIN_8
#define o3_GPIO_Port GPIOE
#define o4_Pin GPIO_PIN_9
#define o4_GPIO_Port GPIOE
#define o5_Pin GPIO_PIN_10
#define o5_GPIO_Port GPIOE
#define o6_Pin GPIO_PIN_11
#define o6_GPIO_Port GPIOE
#define o7_Pin GPIO_PIN_12
#define o7_GPIO_Port GPIOE
#define o8_Pin GPIO_PIN_13
#define o8_GPIO_Port GPIOE
#define o9_Pin GPIO_PIN_14
#define o9_GPIO_Port GPIOE
#define o10_Pin GPIO_PIN_15
#define o10_GPIO_Port GPIOE
#define i2c2_scl_pca9685_Pin GPIO_PIN_10
#define i2c2_scl_pca9685_GPIO_Port GPIOB
#define i2c2_sda_pca9685_Pin GPIO_PIN_11
#define i2c2_sda_pca9685_GPIO_Port GPIOB
#define o11_Pin GPIO_PIN_14
#define o11_GPIO_Port GPIOD
#define o12_Pin GPIO_PIN_15
#define o12_GPIO_Port GPIOD
#define MH_RST_Pin GPIO_PIN_2
#define MH_RST_GPIO_Port GPIOD
#define MH_CS_Pin GPIO_PIN_3
#define MH_CS_GPIO_Port GPIOD
#define MH_DC_Pin GPIO_PIN_4
#define MH_DC_GPIO_Port GPIOD
#define i2c1_scl_imu_Pin GPIO_PIN_6
#define i2c1_scl_imu_GPIO_Port GPIOB
#define i2c1_sda_imu_Pin GPIO_PIN_7
#define i2c1_sda_imu_GPIO_Port GPIOB
#define in1_Pin GPIO_PIN_8
#define in1_GPIO_Port GPIOB
#define in2_Pin GPIO_PIN_9
#define in2_GPIO_Port GPIOB
#define in3_Pin GPIO_PIN_0
#define in3_GPIO_Port GPIOE
#define in4_Pin GPIO_PIN_1
#define in4_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
