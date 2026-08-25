/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_crc.h"
#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_utils.h"

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
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(const char *errWhere);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PB8_BOOT0_SW1_pdin_Pin LL_GPIO_PIN_8
#define PB8_BOOT0_SW1_pdin_GPIO_Port GPIOB
#define PF0_OSC_IN_LED2_odout_Pin LL_GPIO_PIN_0
#define PF0_OSC_IN_LED2_odout_GPIO_Port GPIOF
#define PF1_OSC_OUT_LED1_odout_Pin LL_GPIO_PIN_1
#define PF1_OSC_OUT_LED1_odout_GPIO_Port GPIOF
#define PA0_SCAN10_odout_Pin LL_GPIO_PIN_0
#define PA0_SCAN10_odout_GPIO_Port GPIOA
#define PA1_SCAN11_odout_Pin LL_GPIO_PIN_1
#define PA1_SCAN11_odout_GPIO_Port GPIOA
#define PA2_USART2_TX_Pin LL_GPIO_PIN_2
#define PA2_USART2_TX_GPIO_Port GPIOA
#define PA3_USART2_RX_Pin LL_GPIO_PIN_3
#define PA3_USART2_RX_GPIO_Port GPIOA
#define PA4_SCAN01_puin_Pin LL_GPIO_PIN_4
#define PA4_SCAN01_puin_GPIO_Port GPIOA
#define PA5_SCAN02_puin_Pin LL_GPIO_PIN_5
#define PA5_SCAN02_puin_GPIO_Port GPIOA
#define PA6_SCAN03_puin_Pin LL_GPIO_PIN_6
#define PA6_SCAN03_puin_GPIO_Port GPIOA
#define PA7_SCAN04_puin_Pin LL_GPIO_PIN_7
#define PA7_SCAN04_puin_GPIO_Port GPIOA
#define PB1_SCAN12_odout_Pin LL_GPIO_PIN_1
#define PB1_SCAN12_odout_GPIO_Port GPIOB
#define PA9_PA11_USB_DM_Pin LL_GPIO_PIN_11
#define PA9_PA11_USB_DM_GPIO_Port GPIOA
#define PA10_PA12_USB_DP_Pin LL_GPIO_PIN_12
#define PA10_PA12_USB_DP_GPIO_Port GPIOA
#define PA13_LED4_odout_Pin LL_GPIO_PIN_13
#define PA13_LED4_odout_GPIO_Port GPIOA
#define PA14_LED3_odout_Pin LL_GPIO_PIN_14
#define PA14_LED3_odout_GPIO_Port GPIOA

	/* USER CODE BEGIN Private defines */

	/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
