/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "usb_device.h"

#include "buildDate.h"
#include "usbd_hid.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
extern	volatile	USBD_HandleTypeDef	hUsbDeviceFS;

///	Human Interface Devices (HID) Specifications and Tools
///	https://www.usb.org/hid
/// 	https://usb.org/document-library/hid-usage-tables-17
///			https://usb.org/sites/default/files/hut1_7.pdf
uint8_t 	codeTable[3][12] = {
	{	0x49, 0x4a, 0x4b,		///	Ins    Home   PgUp
		0x4c, 0x4d, 0x4e,		///	Del    End    PgDn
		0xe0, 0x52, 0xe1,		///	Lctrl  ↑      Lshift
		0x50, 0x51, 0x4f	},	///	←      ↓      →

	{	0x24, 0x25, 0x26,		///	7      8      9
		0x21, 0x22, 0x23,		///	4      5      6
		0x1e, 0x1f, 0x20,		///	1      2      3
		0x27, 0x2a, 0x28	},	///	0      Bspace Enter

	{	0x43, 0x44, 0x45,		///	F10    F11    F12
		0x40, 0x41, 0x42,		///	F7     F8     F9
		0x3d, 0x3e, 0x3f,		///	F4     F5     F6
		0x3a, 0x3b, 0x3c	},	///	F1     F2     F3
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile	uint8_t 	usbSOFflag,	///	++ed in USB SOF (USBD_LL_SOF() in usbd_core.c)
						lockLED;	///	Num Lock, Caps Lock, Scroll Lock, ... (USBD_HID_Init(), usbdHIDdataOut() in usbd_hid.c)
uint8_t 	u8, v8, prevLled, scanline, kbuf[12], mbuf, keyMode, curStat[16];
uint16_t	keyStat, prevKstat;
uint32_t	sysID;
uint64_t	reportBuff;
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
	SysTick->CTRL = 0;	///	Disable sysTick
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	__HAL_RCC_DMA1_CLK_ENABLE();	///MX_DMA_Init();
	MX_USART2_UART_Init();
	usart2xmitDMA(bldatePtr, 35);	///	Greetings

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);	///MX_CRC_Init();
	CRC->CR |= CRC_CR_RESET;
	while (CRC->CR & CRC_CR_RESET)
		__NOP();
	CRC->DR = *((uint32_t *)DBGMCU_BASE);
	CRC->DR = *((uint32_t *)(UID_BASE + 8));
	CRC->DR = *((uint32_t *)(UID_BASE + 4));
	CRC->DR = *((uint32_t *)(UID_BASE));
	sysID = CRC->DR;

	while (DMA1_Channel4->CNDTR)
		__NOP();
	MX_USB_DEVICE_Init();
	while (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
		__NOP();

	/* USER CODE BEGIN 2 */
	LL_GPIO_SetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port, PF0_OSC_IN_LED2_odout_Pin);	///	LED2 off
	LL_GPIO_SetOutputPin(PA14_LED3_odout_GPIO_Port, PA14_LED3_odout_Pin);	///	LED3 off
	curStat[1] = curStat[8] = curStat[15] = ' ';
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		while (!usbSOFflag)
			__NOP();
		/* USER CODE END WHILE */

		usbSOFflag--;
		LL_GPIO_TogglePin(PA13_LED4_odout_GPIO_Port, PA13_LED4_odout_Pin);

		///	Check lock LEDs
		if (prevLled != lockLED && !(DMA1_Channel4->CNDTR))
		{
			///	Notifies the status of the lock LEDs via USART2
			curStat[0] =
			curStat[2] =
			curStat[3] =
			curStat[13] =
			curStat[14] = ' ';

			curStat[4] = (lockLED & (1 << 0)) ? '+' : '_';	///	Num lock
			curStat[5] = (lockLED & (1 << 1)) ? '+' : '_';	///	Caps lock
			curStat[6] = (lockLED & (1 << 2)) ? '+' : '_';	///	Scroll lock
			curStat[7] = (lockLED & (1 << 3)) ? '+' : '_';
			curStat[9] = (lockLED & (1 << 4)) ? '+' : '_';
			curStat[10] = (lockLED & (1 << 5)) ? '+' : '_';
			curStat[11] = (lockLED & (1 << 6)) ? '+' : '_';
			curStat[12] = (lockLED & (1 << 7)) ? '+' : '_';
			usart2xmitDMA(curStat, 16);

			prevLled = lockLED;
		}

		///	Check mode button
		mbuf <<= 1;
		if (LL_GPIO_IsInputPinSet(PB8_BOOT0_SW1_pdin_GPIO_Port, PB8_BOOT0_SW1_pdin_Pin))
			mbuf |= 1;
		if (!mbuf)
			keyStat &= 0x7fff;
		else if (mbuf == 0xff && !(keyStat & 0x8000))
		{
			keyStat |= 0x8000;

			///	Keypad mode change (inc)
			switch (keyMode)
			{
				case 0:
					LL_GPIO_SetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
							PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 off
					LL_GPIO_ResetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
							PF0_OSC_IN_LED2_odout_Pin);	///	LED2 On
					keyMode++;
					break;
				case 1:
					LL_GPIO_SetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
							PF0_OSC_IN_LED2_odout_Pin);	///	LED2 off
					LL_GPIO_ResetOutputPin(PA14_LED3_odout_GPIO_Port,
							PA14_LED3_odout_Pin);	///	LED3 On
					keyMode++;
					break;
				default:
					LL_GPIO_SetOutputPin(PA14_LED3_odout_GPIO_Port,
							PA14_LED3_odout_Pin);	///	LED3 off
					LL_GPIO_ResetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
							PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 On
					keyMode = 0;
					break;
			}
		}

		///	Check USART2 Rx
		if (keyStat == prevKstat && (USART2->ISR & USART_ISR_RXNE))
		{
			if ((u8 = USART2->RDR) <= ' ')
			{
				if (!(DMA1_Channel4->CNDTR))
					usart2xmitDMA(bldatePtr, 35);	///	Notifies firmware information via USART2
			}
			else if (u8 & 1)	///	Keypad mode change (inc)
			{
				switch (keyMode)
				{
					case 0:
						LL_GPIO_SetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
								PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 off
						LL_GPIO_ResetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
								PF0_OSC_IN_LED2_odout_Pin);	///	LED2 On
						keyMode++;
						break;
					case 1:
						LL_GPIO_SetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
								PF0_OSC_IN_LED2_odout_Pin);	///	LED2 off
						LL_GPIO_ResetOutputPin(PA14_LED3_odout_GPIO_Port,
								PA14_LED3_odout_Pin);	///	LED3 On
						keyMode++;
						break;
					default:
						LL_GPIO_SetOutputPin(PA14_LED3_odout_GPIO_Port,
								PA14_LED3_odout_Pin);	///	LED3 off
						LL_GPIO_ResetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
								PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 On
						keyMode = 0;
						break;
				}
			}
			else	///	Keypad mode change (dec)
			{
				switch (keyMode)
				{
					case 0:
						LL_GPIO_SetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
								PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 off
						LL_GPIO_ResetOutputPin(PA14_LED3_odout_GPIO_Port,
								PA14_LED3_odout_Pin);	///	LED3 On
						keyMode = 2;
						break;
					case 1:
						LL_GPIO_SetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
								PF0_OSC_IN_LED2_odout_Pin);	///	LED2 off
						LL_GPIO_ResetOutputPin(PF1_OSC_OUT_LED1_odout_GPIO_Port,
								PF1_OSC_OUT_LED1_odout_Pin);	///	LED1 On
						keyMode--;
						break;
					default:
						LL_GPIO_SetOutputPin(PA14_LED3_odout_GPIO_Port,
								PA14_LED3_odout_Pin);	///	LED3 off
						LL_GPIO_ResetOutputPin(PF0_OSC_IN_LED2_odout_GPIO_Port,
								PF0_OSC_IN_LED2_odout_Pin);	///	LED2 On
						keyMode--;
						break;
				}
			}
		}

		///	Scan and check keys
		u8 = GPIOA->IDR;	///	LOW bit means the key is ON.
		switch (scanline)
		{
			case 0:
				///	Prepare for next scan
				LL_GPIO_SetOutputPin(PA0_SCAN10_odout_GPIO_Port, PA0_SCAN10_odout_Pin);
				LL_GPIO_ResetOutputPin(PA1_SCAN11_odout_GPIO_Port, PA1_SCAN11_odout_Pin);

				if ( !(kbuf[0] = ((kbuf[0] << 1) | (u8 & 0x10))) )			///	Row1,Col1: Ins
					keyStat |= (1 << 0);
				else if (kbuf[0] == 0xf0)
					keyStat &= ~(1 << 0);

				if ( !(kbuf[3] = ((kbuf[3] << 1) | ((u8 >> 1) & 0x10))) )	///	Row2,Col1: Del
					keyStat |= (1 << 3);
				else if (kbuf[3] == 0xf0)
					keyStat &= ~(1 << 3);

				if ( !(kbuf[6] = ((kbuf[6] << 1) | ((u8 >> 2) & 0x10))) )	///	Row3,Col1: Lctrl
					keyStat |= (1 << 6);
				else if (kbuf[6] == 0xf0)
					keyStat &= ~(1 << 6);

				if ( !(kbuf[9] = ((kbuf[9] << 1) | ((u8 >> 3) & 0x10))) )	///	Row4,Col1: ←
					keyStat |= (1 << 9);
				else if (kbuf[9] == 0xf0)
					keyStat &= ~(1 << 9);

				scanline++;
				break;

			case 1:
				///	Prepare for next scan
				LL_GPIO_SetOutputPin(PA1_SCAN11_odout_GPIO_Port, PA1_SCAN11_odout_Pin);
				LL_GPIO_ResetOutputPin(PB1_SCAN12_odout_GPIO_Port, PB1_SCAN12_odout_Pin);

				if ( !(kbuf[1] = ((kbuf[1] << 1) | (u8 & 0x10))) )			///	Row1,Col2: Home
					keyStat |= (1 << 1);
				else if (kbuf[1] == 0xf0)
					keyStat &= ~(1 << 1);

				if ( !(kbuf[4] = ((kbuf[4] << 1) | ((u8 >> 1) & 0x10))) )	///	Row2,Col2: End
					keyStat |= (1 << 4);
				else if (kbuf[4] == 0xf0)
					keyStat &= ~(1 << 4);

				if ( !(kbuf[7] = ((kbuf[7] << 1) | ((u8 >> 2) & 0x10))) )	///	Row3,Col2: ↑
					keyStat |= (1 << 7);
				else if (kbuf[7] == 0xf0)
					keyStat &= ~(1 << 7);

				if ( !(kbuf[10] = ((kbuf[10] << 1) | ((u8 >> 3) & 0x10))) )	///	Row4,Col2: ↓
					keyStat |= (1 << 10);
				else if (kbuf[10] == 0xf0)
					keyStat &= ~(1 << 10);

				scanline++;
				break;

			default:
				///	Prepare for next scan
				LL_GPIO_SetOutputPin(PB1_SCAN12_odout_GPIO_Port, PB1_SCAN12_odout_Pin);
				LL_GPIO_ResetOutputPin(PA0_SCAN10_odout_GPIO_Port, PA0_SCAN10_odout_Pin);

				if ( !(kbuf[2] = ((kbuf[2] << 1) | (u8 & 0x10))) )			///	Row1,Col3: PgUp
					keyStat |= (1 << 2);
				else if (kbuf[2] == 0xf0)
					keyStat &= ~(1 << 2);

				if ( !(kbuf[5] = ((kbuf[5] << 1) | ((u8 >> 1) & 0x10))) )	///	Row2,Col3: PgDn
					keyStat |= (1 << 5);
				else if (kbuf[5] == 0xf0)
					keyStat &= ~(1 << 5);

				if ( !(kbuf[8] = ((kbuf[8] << 1) | ((u8 >> 2) & 0x10))) )	///	Row3,Col3: Lshift
					keyStat |= (1 << 8);
				else if (kbuf[8] == 0xf0)
					keyStat &= ~(1 << 8);

				if ( !(kbuf[11] = ((kbuf[11] << 1) | ((u8 >> 3) & 0x10))) )	///	Row4,Col3: →
					keyStat |= (1 << 11);
				else if (kbuf[11] == 0xf0)
					keyStat &= ~(1 << 11);
				
				scanline = 0;
				break;
		}

		///	Check for report
		if (keyStat != prevKstat)
		{
			reportBuff = (uint64_t)0;
			for (u8 = 0, v8 = 2; u8 < 12 && v8 < (2 + 6); u8++)
			{
				if (keyStat & (1 << u8))
				{
					if (codeTable[keyMode][u8] >= 0xe0)	///	modifier
					{
						if (codeTable[keyMode][u8] <= 0xe7)
							((uint8_t *)&reportBuff)[0] |=
									1 << (codeTable[keyMode][u8] - 0xe0);
					}
					else
						((uint8_t *)&reportBuff)[v8++] = codeTable[keyMode][u8];
				}
			}
			if (u8 < 12)	///	Phantom state
			{
				((uint16_t *)&reportBuff)[1] = 0x0101;
				((uint32_t *)&reportBuff)[1] = 0x01010101;
			}

			while ( ((USBD_HID_HandleTypeDef *)hUsbDeviceFS.pClassData)
					->state != HID_IDLE )
				__NOP();
			USBD_HID_SendReport((USBD_HandleTypeDef *)&hUsbDeviceFS,
					(uint8_t *)&reportBuff, sizeof (reportBuff));

			if (!(DMA1_Channel4->CNDTR))
			{
				///	Reports the status of the keys to USART2
				curStat[0] = (keyStat & 0x8000) ? '*' : '-';	///	Mode button
				curStat[2] = (keyStat & (1 << 0)) ? '0' : '-';
				curStat[3] = (keyStat & (1 << 1)) ? '1' : '-';
				curStat[4] = (keyStat & (1 << 2)) ? '2' : '-';
				curStat[5] = (keyStat & (1 << 3)) ? '3' : '-';
				curStat[6] = (keyStat & (1 << 4)) ? '4' : '-';
				curStat[7] = (keyStat & (1 << 5)) ? '5' : '-';
				curStat[9] = (keyStat & (1 << 6)) ? '6' : '-';
				curStat[10] = (keyStat & (1 << 7)) ? '7' : '-';
				curStat[11] = (keyStat & (1 << 8)) ? '8' : '-';
				curStat[12] = (keyStat & (1 << 9)) ? '9' : '-';
				curStat[13] = (keyStat & (1 << 10)) ? 'A' : '-';
				curStat[14] = (keyStat & (1 << 11)) ? 'B' : '-';
				usart2xmitDMA(curStat, 16);
			}

			prevKstat = keyStat;
		}

		/* USER CODE BEGIN 3 */
	}

	__NOP();
	__disable_irq();
	__NOP();
	for (;;)
	{
		__NOP();
		NVIC_SystemReset();
		__NOP();
	}

	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
	{
	}
	LL_RCC_HSI48_Enable();

	/* Wait till HSI48 is ready */
	while (LL_RCC_HSI48_IsReady() != 1)
	{
	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI48);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI48)
	{
	}
	LL_SetSystemCoreClock(48000000);

	/* Update the time base */
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
	{
		Error_Handler(NULL);
	}
	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
	LL_CRS_SetSyncDivider(LL_CRS_SYNC_DIV_1);
	LL_CRS_SetSyncPolarity(LL_CRS_SYNC_POLARITY_RISING);
	LL_CRS_SetSyncSignalSource(LL_CRS_SYNC_SOURCE_USB);
	LL_CRS_SetFreqErrorLimit(34);
	LL_CRS_SetHSI48SmoothTrimming(32);

	/* Select SysTick source clock */
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK_DIV8);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

static	uint16_t	errLen;
static	uint32_t	errTme;
static	const	char	*errMsg;

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(const char *errWhere)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();

	LL_GPIO_ResetOutputPin(GPIOF,
			PF1_OSC_OUT_LED1_odout_Pin | PF0_OSC_IN_LED2_odout_Pin);	///	LED 1,2 On
	LL_GPIO_SetOutputPin(GPIOA,
			PA14_LED3_odout_Pin | PA13_LED4_odout_Pin);	///	LED 3,4 off

	if (errWhere)
		for (errMsg = errWhere, errLen = 0; *errMsg; errMsg++, errLen++)
			;	///	errLen = strlen(errWhere)
	else
		errLen = 0;

	if (errLen && !(DMA1_Channel4->CNDTR))
		usart2xmitDMA(errWhere, errLen);

	while (1)
	{
		if (!(++errTme & 0x1fffff))
		{
			LL_GPIO_TogglePin(GPIOF,
					PF1_OSC_OUT_LED1_odout_Pin | PF0_OSC_IN_LED2_odout_Pin);
			LL_GPIO_TogglePin(GPIOA,
					PA14_LED3_odout_Pin | PA13_LED4_odout_Pin);

			if (errLen && !(DMA1_Channel4->CNDTR))
				usart2xmitDMA(errWhere, errLen);
		}
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
