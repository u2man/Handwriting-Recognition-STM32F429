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
#include "crc.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "global.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WINDOWS_WITH		224
#define WINDOWS_HEIGHT		224

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static void Draw_Menu(void);
static void GetPosition(void);
static void Update_ColorSize(void);


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint32_t radius = 2;
char riga[250];

uint8_t  lcd_status = LCD_OK;
uint32_t ts_status = TS_OK;
TS_StateTypeDef  TS_State = {0};
uint16_t x11, y11;
float PxlNet[28*28];
float PxlNet2D[28][28];
uint32_t idx=0;
uint32_t ysrc, xsrc, xstp, ystp, pxl;
uint32_t xold, yold, firstTime;
uint32_t tmp;
uint32_t LCDAddress;
uint32_t *ptr;
uint32_t *mem;
extern uint32_t classification_result;
char riga[250];
volatile uint32_t time_start, time_end, time_diff;
float tdiff[5];
uint32_t xpos, ypos;
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
   uint32_t ledtimer;
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
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_CRC_Init();
  MX_X_CUBE_AI_Init();
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  BSP_LED_On(LED3);
  BSP_LCD_Init();

  /* Layer2 Init */
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER_LAYER1);
  /* Set Foreground Layer */
  BSP_LCD_SelectLayer(1);
  /* Clear the LCD */
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetColorKeying(1, LCD_COLOR_WHITE);
  BSP_LCD_SetLayerVisible(1, DISABLE);

  /* Layer1 Init */
  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);

  /* Set Foreground Layer */
  BSP_LCD_SelectLayer(0);

  /* Enable The LCD */
  BSP_LCD_DisplayOn();

  /* Clear the LCD */
  BSP_LCD_Clear(LCD_COLOR_BLUE);

  /*##-2- Touch screen initialization ########################################*/
  Touchscreen_Calibration();
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  Draw_Menu();
  ledtimer=HAL_GetTick();
  printf("STM32 AI Test\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

 // MX_X_CUBE_AI_Process();
    /* USER CODE BEGIN 3 */
     GetPosition();
     if(HAL_GetTick()-ledtimer>100){
    	 BSP_LED_Toggle(LED3);
    	 ledtimer=HAL_GetTick();
     }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void Draw_Menu(void)
{
  /* Set background Layer */
  BSP_LCD_SelectLayer(0);

  /* Clear the LCD */

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DrawRect((BSP_LCD_GetXSize()/2)-(WINDOWS_WITH/2)-1, (BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2), WINDOWS_WITH+2, WINDOWS_HEIGHT+2);
	/* OK */
	BSP_LCD_FillCircle( BSP_LCD_GetXSize()-50,BSP_LCD_GetYSize()-20, 15);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	sprintf(riga, "OK");
	BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()-50-5,BSP_LCD_GetYSize()-20-6,(uint8_t*)riga,LEFT_MODE);
	/* msg */
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	sprintf(riga, "Write number in the box!");
	BSP_LCD_DisplayStringAt(0,10,(uint8_t*)riga,CENTER_MODE);

}

static void GetPosition(void)
{
  static uint32_t x = 0, y = 0;
  static uint32_t color_heigh;
  static uint32_t color;
  static TS_StateTypeDef  TS_State;

  /* Heigh of color pen */
  color_heigh = 20;


  /* Get Touch screen position */
  BSP_TS_GetState(&TS_State);

  /* Read the coordinate */
  x = Calibration_GetX(TS_State.X);
  y = Calibration_GetX(TS_State.Y);

 if ((TS_State.TouchDetected) & ( x > ((BSP_LCD_GetXSize()/2)-(WINDOWS_WITH/2)-1 + radius)) & ( y > ((BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2)-1 ) ) & ( x < (BSP_LCD_GetXSize()-((BSP_LCD_GetXSize()/2)-(WINDOWS_WITH/2)-1  + radius )) ) & ( y < (BSP_LCD_GetYSize()-((BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2)-1 + radius )) ))
  {
    BSP_LCD_FillCircle((x), (y), 7);
    //BSP_LED_On(LED3);
    BSP_LED_Off(LED4);
  }
  else if ((TS_State.TouchDetected) & ( (x > BSP_LCD_GetXSize()-50) & ( y > (BSP_LCD_GetYSize() - 50) )) & ( x < BSP_LCD_GetXSize()-50+28 ) & ( y < (BSP_LCD_GetYSize()) ))
  {

	 //Get picture'
	  BSP_LED_On(LED4);
	idx=0;
	pxl=0;
	//printf("Read pixel %d to %d\r\n",(BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2)+radius,((BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2))+WINDOWS_HEIGHT+2);
	for ( ysrc=(BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2)+radius,ypos=0; ysrc<((BSP_LCD_GetYSize()/2)-(WINDOWS_HEIGHT/2))+WINDOWS_HEIGHT+2; ysrc+=8,ypos++)
	{
		for ( xsrc=(BSP_LCD_GetXSize()/2)-(WINDOWS_WITH/2),xpos=0; xsrc<((BSP_LCD_GetXSize()/2)-(WINDOWS_WITH/2))+WINDOWS_WITH; xsrc+=8,xpos++)
		{
			for ( ystp=0; ystp<8; ystp++)
			{
				for ( xstp=0; xstp<8; xstp++)
				{
					tmp = BSP_LCD_ReadPixel( xsrc+xstp,ysrc+ystp);
					pxl += (tmp&0xFF);
					//pxl =tmp;
				}
			}
			pxl /= 100;
			if (pxl>0) pxl=255;
			//printf("pixel %d\r\n",pxl);
			//pxl=(0xFF<<24)|(pxl<<16)|(pxl<<8)|pxl;
			PxlNet2D[ypos][xpos] = (float)pxl;
			BSP_LCD_DrawPixel(xsrc/10,ysrc/10+270,(0xFF<<24)|(pxl<<16)|(pxl<<8)|pxl);
			pxl=0;
		}
		//break;

	}
	BSP_LED_Off(LED4);
	printf("Call AI Process xpos %d ypos %d\r\n",xpos, ypos);

	MX_X_CUBE_AI_Process();
	BSP_LCD_SetFont(&Font16);
	printf("The Number is: %lu\r\n",classification_result);
	if ( classification_result <= 9)
	{
		sprintf((char*)riga,"The Number is: %lu",classification_result);
	} else {
		if ( classification_result <= 36)
		{
			sprintf((char*)riga,"The Letter is: %c",(classification_result-10)+65);
		} else {
			sprintf((char*)riga,"The Letter is: %c",(classification_result-36)+97);
		}
	}

	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0,25,(uint8_t*)riga,CENTER_MODE);
	BSP_LCD_SetFont(&Font12);
	//sprintf(riga, "AI process:%2.3f", tdiff[cTIME_MEASURE_1]*0.001);
	//BSP_LCD_DisplayStringAt(0,20,(uint8_t*)riga,CENTER_MODE);

	HAL_Delay(3000);
	//BSP_LCD_Clear(LCD_COLOR_BLACK);
	Draw_Menu();


  }
}

static void Update_ColorSize(void)
{
  static uint32_t color;

  /* Clear the current circle */
  color = BSP_LCD_GetTextColor();
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillCircle(220, (BSP_LCD_GetYSize()-24), 10);
  BSP_LCD_SetTextColor(color);

  /* Update the selected color icon */
  BSP_LCD_FillCircle(220, (BSP_LCD_GetYSize()-24), radius);

  /* Draw black circle */
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(220, (BSP_LCD_GetYSize() - 24), radius);
  BSP_LCD_SetTextColor(color);
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
