/*
 * global.h
 *
 *  Created on: May 25, 2019
 *      Author: Kang Usman
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "crc.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "app_x-cube-ai.h"

#define LCD_FRAME_BUFFER_LAYER0                  (LCD_FRAME_BUFFER+0x130000)
#define LCD_FRAME_BUFFER_LAYER1                  LCD_FRAME_BUFFER
#define CONVERTED_FRAME_BUFFER                   (LCD_FRAME_BUFFER+0x260000)


extern uint8_t  lcd_status;
extern uint32_t ts_status;
extern TS_StateTypeDef  TS_State;
extern uint16_t x11, y11;
//extern float PxlNet[28*28];
//extern float PxlNet2D[28][28];
extern uint32_t idx;
extern uint32_t ysrc, xsrc, xstp, ystp, pxl;
extern uint32_t xold, yold, firstTime;
extern uint32_t tmp;
extern uint32_t LCDAddress;
extern uint32_t *ptr;
extern uint32_t *mem;
//extern uint32_t classification_result;
extern char riga[250];
//volatile uint32_t time_start, time_end, time_diff;
extern float tdiff[5];


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Touchscreen_Calibration (void);
uint16_t Calibration_GetX(uint16_t x);
uint16_t Calibration_GetY(uint16_t y);
uint8_t IsCalibrationDone(void);

#endif /* GLOBAL_H_ */
