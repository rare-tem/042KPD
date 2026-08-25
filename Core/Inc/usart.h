/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

inline	__attribute__((always_inline))
void	usart2xmitDMA(const void *buff, const uint16_t len)
{
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CMAR = (uint32_t)buff;
	DMA1_Channel4->CNDTR = len;
	DMA1->IFCR |= (DMA_IFCR_CTEIF4 | DMA_IFCR_CHTIF4
			| DMA_IFCR_CTCIF4 | DMA_IFCR_CGIF4);
	DMA1_Channel4->CCR |= DMA_CCR_EN;
}

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
