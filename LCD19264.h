#ifndef __LCD19264_H
#define __LCD19264_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"


#define LCD_RST_PIN		RST_Pin
#define LCD_RST_PORT	RST_GPIO_Port

#define LCD_CS_PIN		CS_Pin
#define LCD_CS_PORT		CS_GPIO_Port

#define LCD_DC_PIN		DC_Pin
#define LCD_DC_PORT		DC_GPIO_Port


void initial_lcd(void);
void fill_screen(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color);

void display_graphic_192x64(unsigned char *dp);
void display_graphic(uint8_t page, uint8_t column, uint8_t x ,uint8_t y ,unsigned char *dp);

void display_graphic_32x32(uint8_t page,uint8_t column,unsigned char *dp);
void display_graphic_16x16(uint8_t page,uint8_t column,unsigned char *dp);
void display_graphic_8x8(uint8_t page,uint8_t column,unsigned char *dp);

void display_string_32x32(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text);
void display_string_16x16(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text);
void display_string_8x8(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text);


#endif
