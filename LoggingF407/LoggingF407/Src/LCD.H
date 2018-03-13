#ifndef  __LCD_H
#define  __LCD_H
#include "stm32f4xx_hal.h"
#include "user.h"


//-------------------------��Ļ������������--------------------------------------//
#define LCD_X_SIZE	        176
#define LCD_Y_SIZE	        220

/////////////////////////////////////�û�������///////////////////////////////////	 
//֧�ֺ��������ٶ����л�
#define USE_HORIZONTAL  		1	//�����Ƿ�ʹ�ú��� 		0,��ʹ��.1,ʹ��.

#ifdef USE_HORIZONTAL//��������˺��� 
#define X_MAX_PIXEL	        LCD_Y_SIZE
#define Y_MAX_PIXEL	        LCD_X_SIZE
#else
#define X_MAX_PIXEL	        LCD_X_SIZE
#define Y_MAX_PIXEL	        LCD_Y_SIZE
#endif

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//��ɫ0 3165 00110 001011 00101
#define GRAY1   0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2   0x4208      	//��ɫ2  1111111111011111


#define LCD_CTRL   	  	GPIOD		//����TFT���ݶ˿�
#define LCD_RS         	GPIO_PIN_0//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_PIN_2 //MCU_PB11--->>TFT --CS/CE
#define LCD_RST     	GPIO_PIN_1	//PB10--->>TFT --RST

//Һ�����ƿ���1�������궨��
#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS    
#define	LCD_RS_SET  	LCD_CTRL->BSRR=LCD_RS    
//#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA    
//#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST    
//#define	LCD_LED_SET  	LCD_CTRL->BSRR=LCD_LED   

//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	LCD_CTRL->BSRR=LCD_CS<<16U;    
#define	LCD_RS_CLR  	LCD_CTRL->BSRR=LCD_RS<<16U;    
//#define	LCD_SDA_CLR  	LCD_CTRL->BSRR=LCD_SDA<<16U;    
//#define	LCD_SCL_CLR  	LCD_CTRL->BSRR=LCD_SCL<<16U;    
#define	LCD_RST_CLR  	LCD_CTRL->BSRR=LCD_RST<<16U;    
//#define	LCD_LED_CLR  	LCD_CTRL->BSRR=LCD_LED<<16U; 

struct typFNT_GB162
{
       unsigned char Index[2];	
       char Msk[32];
};

struct typFNT_GB242
{
       unsigned char Index[2];	
       char Msk[72];
};

void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);    
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num);
void Show_IP_Config(USERPRV * _userprv,uint16_t fc,uint16_t bc);
void showimage(const unsigned char *p,uint8_t xpos,uint8_t ypos); 
void Show_One_Line_Parameter(uint16_t xpos,uint16_t ypos,uint8_t p1,uint8_t p2,uint8_t p3,uint8_t p4,uint16_t fc,uint16_t bc);
void Show_One_Minate(USERPRV * _userprv,uint8_t delaytime);
void Show_Num(uint16_t xpos,uint16_t ypos,uint16_t fc,uint16_t bc,uint32_t VAL);
void Show_Num_float(uint16_t xpos,uint16_t ypos,uint16_t fc,uint16_t bc,float VAL);
#endif
