#ifndef __LCD19264_H
#define __LCD19264_H

/****************************************************************/

#include "main.h"
#include "font.h"
#include <stdbool.h>

#define LCD_RST_PIN		LCD_RST_Pin
#define LCD_RST_PORT	LCD_RST_GPIO_Port

#define LCD_CS_PIN		LCD_CS_Pin
#define LCD_CS_PORT		LCD_CS_GPIO_Port

#define LCD_DC_PIN		LCD_DC_Pin
#define LCD_DC_PORT		LCD_DC_GPIO_Port

#define LCD_BL_PIN		BL_Pin
#define LCD_BL_PORT		BL_GPIO_Port



void LCD_Init(void);
void DispFill(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color);
void DispPic(uint8_t page, uint8_t column, uint8_t x ,uint8_t y ,const unsigned char *dp);
void DispChar(uint8_t page, uint8_t column, char ascii, sFONT* font);
void DispString(uint8_t page, uint8_t column, const char* text, sFONT* font);
	
void DispBacklight(uint8_t lumi);
void DispContrast(uint8_t contrast);
void DispPower(uint8_t mode);

/****************************************************************/

#endif
