/****************************************************************/
// JLX19264G-925-PN-S
// ���нӿ�
// ����IC : ST7525
// ˮƽ��ʾ��192x64����
// ����6 4�У�8��Ϊһ��page������8��page��д��ҳ����ʱ�����ϵ���д�룬������������ض�Ӧ�������λ��
// ����192�У���192column��д��˳�������ң�д�����һ�в����Զ����С�
//
// ȡͼ��ģʹ��img2lcd������ˮƽ���ֽڴ�ֱ���ֽ������ݵ��򣬲�����ͼ��ͷ����
// ȡ��ģʹ��Pctolcd2002�����룬����ʽ������
/****************************************************************/


#include "LCD19264.h"
#include "spi.h"
//#include "stmcompat.h"

/****************************************************************/



/****************************************************************/


/**
 * @brief  дָ��
 * @param  cmd ָ��
 */
void transfer_command(uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,&cmd,1,1000);
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
	HAL_SPI_Transmit(&hspi2,&data,1,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  д�������
 * @param  data ����ָ��
 * @param  length ���ݳ���
 */
void transfer_muilt_data(uint8_t * data, uint16_t length)
{
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi2,data,length,1000);
	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET);
}


/**
 * @brief  LCD��ʼ��
 */
void LCD_Init()
{
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET); //�͵�ƽ��λ
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET);//��λ���
	HAL_Delay(100);
	transfer_command(0xE2); //����λ
	HAL_Delay(200);
	transfer_command(0xA0); //ˢ����76fps
	transfer_command(0xEB); //1/9 ƫѹ�ȣ�bias��
	transfer_command(0xC4); //����ɨ��˳�򣺴��ϵ���
	transfer_command(0x81); //΢���Աȶ�
	transfer_command(0x79); //΢���Աȶȵ�ֵ�������÷�Χ0x00��0xFF
	transfer_command(0xAF); //����ʾ
	DAC_Start(2);
	HAL_Delay(5);
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
 * @param  page ��ʼҳ
 * @param  column ��ʼ��
 * @param  x ����
 * @param  y �߶�
 * @param  color ��ɫ 0:��ɫ 1:��ɫ
 */
void DispFill(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color)
{
	uint8_t i,j;
	uint8_t height = (uint8_t)(y/8);
	
	for(i=0;i<height;i++)
	{
		lcd_address(page+i,column);
		if(color == 0)
		{
			for(j=0;j<x;j++)
			{
				transfer_data(0x00);
			}
		}
		else
		{
			for(j=0;j<x;j++)
			{
				transfer_data(0xFF);
			}
		}
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
void DispPic(uint8_t page, uint8_t column, uint8_t x ,uint8_t y ,const unsigned char *dp)
{
	uint8_t length, height;
	length = x;
	height = (uint8_t)(y/8);
	uint8_t i;
	for(i=0;i<height;i++)
	{
		lcd_address(page+i,column);
		transfer_muilt_data((uint8_t *)dp, length);
		dp = dp + length;
	}
}


/**
 * @brief  ��ʾһ��ASCII�ַ�
 * @param  page ҳ
 * @param  column ��
 * @param  ascii �ַ�
 * @param  font ����
 */
void DispChar(uint8_t page, uint8_t column, char ascii, sFONT* font)
{
	uint8_t i;
	uint16_t char_offset = (ascii - ' ') * font->Height * font->Width / 8;
	const unsigned char* ptr = &font->table[char_offset];
	
	for(i=0;i<(font->Height/8);i++)
	{
		lcd_address(page+i,column);
		transfer_muilt_data((uint8_t *)ptr, font->Width);
		ptr = ptr + font->Width;
	}
}


/**
 * @brief  ��ʾASCII�ַ���
 * @param  page ҳ
 * @param  column ��
 * @param  text �ַ���
 * @param  font ����
 */
void DispString(uint8_t page, uint8_t column, const char* text, sFONT* font)
{
	const char* p_text = text;
	uint8_t x = column;
	
	while(*p_text != 0)
	{
		DispChar(page,x,*p_text,font);
		x = x + font->Width;
		p_text++;
	}
}

/****************************************************************/

/**
 * @brief  LCD����ģʽ
 * @param  lumi 0-100 ����ǿ��
 */
void DispBacklight(uint8_t lumi)
{
	//DAC_OutVol(2, lumi);	//���⿪��
}

/**
 * @brief  LCD�Աȶ�
 * @param  contrast 0-255/norm:121
 */
void DispContrast(uint8_t contrast)
{
	transfer_command(0x81); //���Աȶ�
	transfer_command(contrast); //���Աȶȵ�ֵ�������÷�Χ0x00��0xFF
}

/**
 * @brief  LCD��Դģʽ
 * @param  mode 0:������ʾ 1:�ر�ģʽ
 */
void DispPower(uint8_t mode)
{
	if(mode == 0)
		transfer_command(0xAF);	//PD = 1 �˳�ʡ��ģʽ
	else if (mode ==1)
	{
		transfer_command(0xAE);	//PD = 0 ʡ��ģʽ
		//DispBacklight(0);		//�رձ���
	}
	HAL_Delay(300);	//��С250ms�Թرջ���LCD��ѹ��·
}


