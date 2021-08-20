/*****************************************************/
// JLX19264G-925-PN-S
// ���нӿ�
// ����IC : ST7525
// ˮƽ��ʾ��192x64����
// ����6 4�У�8��Ϊһ��page������8��page��д��ҳ����ʱ�����ϵ���д�룬������������ض�Ӧ�������λ��
// ����192�У���192column��д��˳�������ң�д�����һ�в����Զ����С�
//
// LCDȡ��ģ���ʹ��img2lcd������ˮƽ���ֽڴ�ֱ���ֽ������ݵ��򣬲�����ͼ��ͷ����
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
 * @brief  дָ��
 * @param  cmd ָ��
 */
void transfer_command(uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&cmd,1,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  д����
 * @param  data ����
 */
void transfer_data(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1,&data,1,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  LCD��ʼ��
 */
void initial_lcd()
{
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET); //�͵�ƽ��λ
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET);//��λ���
	HAL_Delay(200);
	transfer_command(0xe2); //��λ
	HAL_Delay(200);
	transfer_command(0x2f); //���ڲ���ѹ
	HAL_Delay(200);
	transfer_command(0xa0); //
	transfer_command(0x81); //΢���Աȶ�
	transfer_command(0x79); //΢���Աȶȵ�ֵ�������÷�Χ0x00��0xFF
	transfer_command(0xeb); //1/9 ƫѹ�ȣ�bias��
	transfer_command(0xc4); //����ɨ��˳�򣺴��ϵ���
	transfer_command(0xaf); //����ʾ
}


/**
 * @brief  ����LCD�Դ�ָ��
 * @param  page ҳ��[1-8]
 * @param  column ����[1-192]
 */
void lcd_address(uint8_t page,uint8_t column)
{
	column=column-1; //��1�У���LCD����IC���ǵ�0�У������������ȥ1
	page=page-1;		 //��1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1
	transfer_command(0xb0+page); //����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ
	transfer_command(((column>>4)&0x0f)+0x10); //�����е�ַ�ĸ�4λ
	transfer_command(column&0x0f); 						 //�����е�ַ�ĵ�4λ
}


/**
 * @brief  �����Ļ���߶���Ϊ8�ı�����
 * @param  page ҳ
 * @param  column ��
 * @param  x ����
 * @param  y �߶�
 * @param  color ��ɫ 0:��ɫ 1:��ɫ
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
 * @brief  ��ʾ192x64����ͼ��
 * @param  *dp ����ָ��
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
 * @brief  ��ʾ32x32ͼ��
 * @param  page ҳ
 * @param  column ��
 * @param  *dp ͼ��ָ��
 */
void display_graphic_32x32(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<4;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<32;i++)
		{
			transfer_data(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}


/**
 * @brief  ��ʾ16x16ͼ��
 * @param  page ҳ
 * @param  column ��
 * @param  *dp ͼ��ָ��
 */
void display_graphic_16x16(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
			transfer_data(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

/**
 * @brief  ��ʾ8x8ͼ��
 * @param  page ҳ
 * @param  column ��
 * @param  *dp ͼ��ָ��
 */
void display_graphic_8x8(uint8_t page,uint8_t column,unsigned char *dp)
{
	uint8_t i;
	lcd_address(page,column);
	for (i=0;i<8;i++)
	{
		transfer_data(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
		dp++;
	}
}


/**
 * @brief  д��һ��32x32������ַ������ַ���������躬�д��֣�
 * @param  page ҳ
 * @param  column ��
 * @param  reverse ������ʾ 0:������ʾ 1:��ת��ʾ 
 * @param  *dp �ַ�������ָ�� 65535Ϊ����
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
				transfer_data(data); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
				x++;
			}
		}
		k++;
		x = 0;
		col = col + 32;
	}
	
}


/**
 * @brief  д��һ��16x16������ַ������ַ���������躬�д��֣�
 * @param  page ҳ
 * @param  column ��
 * @param  reverse ������ʾ 0:������ʾ 1:��ת��ʾ 
 * @param  *dp �ַ�������ָ�� 65535Ϊ����
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
				transfer_data(data); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
				x++;
			}
		}
		k++;
		x = 0;
		col = col + 16;
	}
	
}


/**
 * @brief  д��һ��8x8������ַ������ַ���������躬�д��֣�
 * @param  page ҳ
 * @param  column ��
 * @param  reverse ������ʾ 0:������ʾ 1:��ת��ʾ 
 * @param  *dp �ַ�������ָ�� 65535Ϊ����
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
			transfer_data(data); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			x++;
		}
		k++;
		x = 0;
		col = col + 8;
	}
	
}


/**
 * @brief  д�������С��ͼ�񣨸߶���Ϊ8�ı�����
 * @param  page ҳ
 * @param  column ��
 * @param  x ����
 * @param  y �߶�
 * @param  *dp ͼ��ָ��
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