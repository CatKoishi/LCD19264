/*****************************************************/
// JLX19264G-925-PN-S
// 串行接口
// 驱动IC : ST7525
// 水平显示，192x64像素
// 共有6 4行，8行为一个page，共有8个page，写入页数据时，由上到下写入，及最上面的像素对应数据最低位。
// 共有192列，即192column，写入顺序由左到右，写完最后一列不会自动换行。
//
// LCD取字模软件使用img2lcd，数据水平，字节垂直，字节内数据倒序，不包含图像头数据
//
//
//
//
//
/*****************************************************/

#include "stm32f4xx_hal.h"
#include "LCD19264.h"
#include "spi.h"
#include "Font.h"


/**
 * @brief  写指令
 * @param  cmd 指令
 */
void transfer_command(uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&cmd,1,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  写数据
 * @param  data 数据
 */
void transfer_data(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1,&data,1,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  LCD初始化
 */
void initial_lcd()
{
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET); //低电平复位
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET);//复位完毕
	HAL_Delay(200);
	transfer_command(0xe2); //软复位
	HAL_Delay(200);
	transfer_command(0x2f); //打开内部升压
	HAL_Delay(200);
	transfer_command(0xa0); //
	transfer_command(0x81); //微调对比度
	transfer_command(0x79); //微调对比度的值，可设置范围0x00～0xFF
	transfer_command(0xeb); //1/9 偏压比（bias）
	transfer_command(0xc4); //行列扫描顺序：从上到下
	transfer_command(0xaf); //开显示
}


/**
 * @brief  设置LCD显存指针
 * @param  page 页数[1-8]
 * @param  column 列数[1-192]
 */
void lcd_address(uint8_t page,uint8_t column)
{
	column=column-1; //第1列，在LCD驱动IC里是第0列，所以在这里减去1
	page=page-1;		 //第1页，在LCD驱动IC里是第0页，所以在这里减去1
	transfer_command(0xb0+page); //设置页地址。每页是8行。一个画面的64行被分成8个页
	transfer_command(((column>>4)&0x0f)+0x10); //设置列地址的高4位
	transfer_command(column&0x0f); 						 //设置列地址的低4位
}


/**
 * @brief  填充屏幕（高度需为8的倍数）
 * @param  page 页
 * @param  column 列
 * @param  x 长度
 * @param  y 高度
 * @param  color 颜色 0:白色 1:黑色
 */
void fill_screen(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color)
{
	uint8_t length, height;
	length = x;
	height = (uint8_t)(y/8);
	uint8_t i,j;
	for(i=0;i<height;i++)
	{
		lcd_address(page+i,column);
		if(color == 0)
		{
			for(j=0;j<length;j++)
			{
				transfer_data(0x00);
			}
		}
		else
		{
			for(j=0;j<length;j++)
			{
				transfer_data(0xFF);
			}
		}
	}
}


/**
 * @brief  显示192x64像素图像
 * @param  *dp 数组指针
 */
void display_graphic_192x64(unsigned char *dp)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		lcd_address(i+1,1);
		for(j=0;j<192;j++)
		{
			transfer_data(*dp);
			dp++;
		}
	}
}


/**
 * @brief  显示32x32图标
 * @param  page 页
 * @param  column 列
 * @param  *dp 图标指针
 */
void display_graphic_32x32(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<4;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<32;i++)
		{
			transfer_data(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			dp++;
		}
	}
}


/**
 * @brief  显示16x16图标
 * @param  page 页
 * @param  column 列
 * @param  *dp 图标指针
 */
void display_graphic_16x16(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
			transfer_data(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			dp++;
		}
	}
}

/**
 * @brief  显示8x8图标
 * @param  page 页
 * @param  column 列
 * @param  *dp 图标指针
 */
void display_graphic_8x8(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i;
	lcd_address(page,column);
	for (i=0;i<8;i++)
	{
		transfer_data(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
		dp++;
	}
}


/**
 * @brief  写入一组32x32点阵的字符串（字符串表格中需含有此字）
 * @param  page 页
 * @param  column 列
 * @param  reverse 反相显示 0:正常显示 1:反转显示 
 * @param  *dp 字符串索引指针 65535为结束
 */
void display_string_32x32(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text)
{
	uint8_t i,j, k = 0 , x = 0;
	uint8_t rol = page ,col = column ;
	uint8_t data;
	
	while(text[k] != 65535)
	{
		for(j=0;j<4;j++)
		{
			lcd_address(rol+j,col);
			for (i=0;i<32;i++)
			{
				if(reverse == 0)
					data = FONT32[text[k]*128+x];
				else 
					data = ~(FONT32[text[k]*128+x]);
				transfer_data(data); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
				x++;
			}
		}
		k++;
		x = 0;
		col = col + 32;
	}
	
}


/**
 * @brief  写入一组16x16点阵的字符串（字符串表格中需含有此字）
 * @param  page 页
 * @param  column 列
 * @param  reverse 反相显示 0:正常显示 1:反转显示 
 * @param  *dp 字符串索引指针 65535为结束
 */
void display_string_16x16(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text)
{
	uint8_t i,j, k = 0 , x = 0;
	uint8_t rol = page ,col = column ;
	uint8_t data;
	
	while(text[k] != 65535)
	{
		for(j=0;j<2;j++)
		{
			lcd_address(rol+j,col);
			for (i=0;i<16;i++)
			{
				if(reverse == 0)
					data = FONT16[text[k]*32+x];
				else 
					data = ~(FONT16[text[k]*32+x]);
				transfer_data(data); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
				x++;
			}
		}
		k++;
		x = 0;
		col = col + 16;
	}
	
}


/**
 * @brief  写入一组8x8点阵的字符串（字符串表格中需含有此字）
 * @param  page 页
 * @param  column 列
 * @param  reverse 反相显示 0:正常显示 1:反转显示 
 * @param  *dp 字符串索引指针 65535为结束
 */
void display_string_8x8(uint8_t page,uint8_t column,uint8_t reverse, uint16_t *text)
{
	uint8_t i ,k = 0 ,x = 0;
	uint8_t rol = page ,col = column ;
	uint8_t data;
	
	while(text[k] != 65535)
	{
		lcd_address(rol,col);
		for (i=0;i<8;i++)
		{
			if(reverse == 0)
				data = FONT8[text[k]*32+x];
			else 
				data = ~(FONT8[text[k]*32+x]);
			transfer_data(data); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			x++;
		}
		k++;
		x = 0;
		col = col + 8;
	}
	
}


/**
 * @brief  写入任意大小的图像（高度需为8的倍数）
 * @param  page 页
 * @param  column 列
 * @param  x 长度
 * @param  y 高度
 * @param  *dp 图像指针
 */
void display_graphic(uint8_t page, uint8_t column, uint8_t x ,uint8_t y ,unsigned char *dp)
{
	uint8_t length, height;
	length = x;
	height = (uint8_t)(y/8);
	uint8_t i,j;
	for(i=0;i<height;i++)
	{
		lcd_address(page+i,column);
		for(j=0;j<length;j++)
		{
			transfer_data(*dp);
			dp++;
		}
	}
}