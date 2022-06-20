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
#include "timer.h"
#include "oledlib.h"
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim4;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED8_SCL_SCLK_Pin GPIO_PIN_10
#define OLED8_SCL_SCLK_GPIO_Port GPIOB
#define OLED8_SDA_SDO_Pin GPIO_PIN_11
#define OLED8_SDA_SDO_GPIO_Port GPIOB
#define OLED2_CS_Pin GPIO_PIN_6
#define OLED2_CS_GPIO_Port GPIOC
#define ZIKU_CS_Pin GPIO_PIN_7
#define ZIKU_CS_GPIO_Port GPIOC
#define OLED0_CS_Pin GPIO_PIN_8
#define OLED0_CS_GPIO_Port GPIOC
#define OLED3_CS_Pin GPIO_PIN_9
#define OLED3_CS_GPIO_Port GPIOC
#define OLED0_2_3_RST_Pin GPIO_PIN_8
#define OLED0_2_3_RST_GPIO_Port GPIOA
#define OLED0_2_3_DC_Pin GPIO_PIN_9
#define OLED0_2_3_DC_GPIO_Port GPIOA
#define OLED0_2_3_SCL_SCLK_Pin GPIO_PIN_10
#define OLED0_2_3_SCL_SCLK_GPIO_Port GPIOC
#define ZIKU_MISO_Pin GPIO_PIN_11
#define ZIKU_MISO_GPIO_Port GPIOC
#define OLED0_2_3_SDA_SDO_Pin GPIO_PIN_12
#define OLED0_2_3_SDA_SDO_GPIO_Port GPIOC
#define OLED5_SCL_SCLK_Pin GPIO_PIN_0
#define OLED5_SCL_SCLK_GPIO_Port GPIOD
#define OLED5_SDA_SDO_Pin GPIO_PIN_1
#define OLED5_SDA_SDO_GPIO_Port GPIOD
#define OLED6_SCL_SCLK_Pin GPIO_PIN_2
#define OLED6_SCL_SCLK_GPIO_Port GPIOD
#define OLED6_SDA_SDO_Pin GPIO_PIN_3
#define OLED6_SDA_SDO_GPIO_Port GPIOD
#define OLED7_SCL_SCLK_Pin GPIO_PIN_4
#define OLED7_SCL_SCLK_GPIO_Port GPIOD
#define OLED7_SDA_SDO_Pin GPIO_PIN_5
#define OLED7_SDA_SDO_GPIO_Port GPIOD
#define OLED1_SCL_SCLK_Pin GPIO_PIN_5
#define OLED1_SCL_SCLK_GPIO_Port GPIOB
#define OLED1_SDA_SDO_Pin GPIO_PIN_6
#define OLED1_SDA_SDO_GPIO_Port GPIOB
#define OLED4_SCL_SCLK_Pin GPIO_PIN_7
#define OLED4_SCL_SCLK_GPIO_Port GPIOB
#define OLED4_SCL_SCLKB8_Pin GPIO_PIN_8
#define OLED4_SCL_SCLKB8_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
