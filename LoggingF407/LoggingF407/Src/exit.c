#include "exit.h"
#include "server.h"
#include "user.h"

void Read_Data_From_FPGA(uint16_t *data_buff,uint16_t num)
{
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
  *(data_buff+num) = DATAIN;
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);
}

void Send_Data_To_FPGA_OneCmd(uint16_t cmd)
{
	GPIOF->MODER = 0x55555555;
	GPIOF->PUPDR = 0X55555555;
	GPIOF->OTYPER = 0X00000000;
	GPIOF->OSPEEDR = 0XAAAAAAAA;
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET); //写请求
	DATAOUT = cmd;
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
//只要一个时钟
//	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
	Delay_us(1);
	 HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET); 
	GPIOF->MODER = 0x00000000;
	GPIOF->PUPDR = 0X55555555;
}

void Send_Data_To_FPGA_DoubleCmd(uint16_t hand,uint16_t cmd)
{
	//切换成双字节命令
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
	
	GPIOF->MODER = 0x55555555;
	GPIOF->PUPDR = 0X55555555;
	GPIOF->OTYPER = 0X00000000;
	GPIOF->OSPEEDR = 0XAAAAAAAA;
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
  DATAOUT = hand;
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
	
  DATAOUT = cmd;
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
  
	Delay_us(1);
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET); 
//只要一个时钟
//	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
	
	GPIOF->MODER = 0x00000000;
	GPIOF->PUPDR = 0x55555555;
	
	//切换成单字节命令
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);	
}


void Set_Baud(uint16_t zcz)
{
	switch(zcz)
	{
		case 1:
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
		break;
		case 2:
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);			
		break;
		case 3:
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
		    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
	  break;
		default :
			  __nop();
		break;
	}
}


