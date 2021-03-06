#include "user.h"
#include "exit.h"
#include "server.h"
#include "lcd.h"
#include  "flash.h"
#include "picture.h"
#include "24cxx.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>

USERPRV m_userprv;
volatile int a = 0;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim4;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;

extern const unsigned char asc16[];
extern const unsigned char sz32[];
extern const struct typFNT_GB162 hz16[];
extern const struct typFNT_GB242 hz24[];
/**
  * @brief us 延时
  * @param  
  *           
  * @retval 
  */

void Delay_us(uint32_t time)
{
	htim2.Instance->CNT = 0;
	__HAL_TIM_ENABLE(&htim2);
	while(htim2.Instance->CNT<(84*time));
	__HAL_TIM_ENABLE(&htim2);
}

/**
  * @brief  ms 延时 
  * @param  time <15887
  *              
  * @retval                                                                                                                    
  */
void Delay_ms(uint32_t time)
{
	htim2.Instance->CNT = 0;
	__HAL_TIM_ENABLE(&htim2);
	while(htim2.Instance->CNT<(84000*time));
	__HAL_TIM_ENABLE(&htim2);
}
/**
  * @brief  全局变量初始化
  * @param  
  *              

  * @retval                                                                                                                    
  */
void Data_Init(USERPRV * _userprv)
{
	_userprv->m_ucUSBRXFlag = 0;
	_userprv->m_ucETHRXFLAG = 0;
	_userprv->m_ucTIM3Count = 0;
	_userprv->m_ucCommendFlag = 0;
	_userprv->m_usSampleWay = 0;
	_userprv->m_usOneMeterPaus = 0;
	_userprv->m_ucRExitflag = 0;
	
	_userprv->m_ucReadMoRenFlag = 0;
	_userprv->m_ucWriteMoRenFlag = 0;
	
	_userprv->m_usOneFramNUM = 0;
	_userprv->m_usSampleSpan = 0;
	_userprv->m_usMoNiQiSpeed = 0;
	_userprv->m_ucTransDataON = 0;
	_userprv->m_ucSlefTestON = 0;
	_userprv->m_ucMoNiQiDirec = 0;
	_userprv->m_usSendCmdVal = 0;
	
	_userprv->m_iTimer4UpdateCnt = 0;

  // _userprv->m_uiBMQSTAVal = 0x7fffffff;
	 _userprv->m_ucMainPageFlag = 1;
	
	_userprv->m_tIPinformation.WriteFlag = 0x97fddb90;
	
  _userprv->m_tInstrument.hand = 0xffffbbaa;
	_userprv->m_tInstrument.end =  0xffffaabb;
	
	_userprv->m_tInstrument_two.hand = 0xffffbbcc;
	_userprv->m_tInstrument_two.end =  0xffffccbb;
	
	_userprv->m_tCommendStack.DATA1 = 0xffff;
	_userprv->m_tCommendStack.DATA2 = 0x97fd;
	
	_userprv->m_tModelDes.hand = 0xffffaadd;
	_userprv->m_tModelDes.type = 0x2;
	_userprv->m_tModelDes.state = 0x4;
	_userprv->m_tModelDes.ID = 0x3002;           ///////////////////////////一个新的产品需要在这里加1
	_userprv->m_tModelDes.end = 0xffffddaa;
	
	_userprv->m_tSystemState.hand = 0xffffbbee;
	_userprv->m_tSystemState.depth = 0x7fffffff;
	_userprv->m_tSystemState.time = 0;
	_userprv->m_tSystemState.strain = 0xffffffff;
	_userprv->m_tSystemState.end = 0xffffeebb;
	
	_userprv->m_tLoggingStaHand.hand = 0xffffabcd;
	_userprv->m_tLoggingStaEnd.end = 0xffffdcba;
	
	_userprv->m_tYA_ZhongZi.hand = 0xffff97fd;
	_userprv->m_tYA_ZhongZi.num = 0x0c;
	_userprv->m_tYA_ZhongZi.type = 0x3006;
	
	_userprv->m_tIPinformation.DNS.data[3] = 144;
	_userprv->m_tIPinformation.DNS.data[2] = 144;
	_userprv->m_tIPinformation.DNS.data[1] = 144;
	_userprv->m_tIPinformation.DNS.data[0] = 144;
	
	_userprv->m_tIPinformation.Severs.data[3] = 192;
	_userprv->m_tIPinformation.Severs.data[2] = 168;
	_userprv->m_tIPinformation.Severs.data[1] = 0;
	_userprv->m_tIPinformation.Severs.data[0] = 100;
	
	_userprv->m_tIPinformation.port = 5050;
	_userprv->m_tIPinformation.WriteFlag = 0X97FDDB90;
	_userprv->m_uiMoniqicnt = 0x7fffffff;
	
	_userprv->m_ucFLUSH_YJP = 0x0;
	
}

/**
  * @brief  读取默认参数 这些参数 是上次 上位机发送了配置命令 后 保存的数据
  * @param 
  *              
  * @retval                                                                                                                    
  */
 unsigned char databuff1[50] = {0x00};
void Read_MoRen_Val(USERPRV * _userprv)//Write_MoRen_Val
{
	AT24CXX_Read(0x00,databuff1,28);
	//AT24CXX_Read(0x04,databuff1,24);
	 _userprv->m_uiBMQSTAVal = (databuff1[3]<<24)+(databuff1[2]<<16)+(databuff1[1]<<8)+(databuff1[0]);    //编码器 起始值 这个值是备用的 目前程序中没有用
	_userprv->m_usSampleSpan = (databuff1[8]<<24)+(databuff1[9]<<16)+(databuff1[10]<<8)+(databuff1[11]);   //采样间隔 	
	//_userprv->m_tModelDes.ID = (databuff1[8]<<24)+(databuff1[9]<<16)+(databuff1[10]<<8)+(databuff1[11]); //模块的ID 0X3001 0x3002 等等
	_userprv->m_usOneFramNUM = (databuff1[16]<<8)+(databuff1[17]);	                                     // 命令类型
	_userprv->m_usOneMeterPaus = (databuff1[18]<<8)+(databuff1[19]);                                     // 每米 脉冲数
	_userprv->m_ucBaud = databuff1[20];                                                                  // 波特率
	_userprv->m_usSampleWay = databuff1[21];                                                             //采样方式
	_userprv->m_ucMoNiQiDirec = databuff1[22];                                                           //模拟器方向
	_userprv->m_ucCommendFlag = databuff1[23];//_userprv->m_ucCommendFlag                                // 标志位 备用
	_userprv->m_usMoNiQiSpeed = (databuff1[24]<<8)+(databuff1[25]);	                                     //模拟器速度
	_userprv->m_ucWaitTime = databuff1[26];                                                              //发送了0x1000 命令 等待所有的数据上传完成 就是这个等待时间
  _userprv->m_ucWaitTime1 = databuff1[27]; 
		
	//上位机仪器检测模式2 每个数之间的等待时间
	if(_userprv->m_usSampleWay == 0x01)
	{
		_userprv->m_fdeepth = _userprv->m_uiBMQSTAVal/100.0;
		_userprv->m_tSystemState.depth = _userprv->m_fdeepth*10;
	}
	else if(_userprv->m_usSampleWay == 0x02)
	{
		_userprv->m_uiBMQVal = _userprv->m_uiBMQSTAVal;
		_userprv->m_fdeepth =  _userprv->m_uiBMQSTAVal/(_userprv->m_usOneMeterPaus*1.0);
		_userprv->m_tSystemState.depth = _userprv->m_fdeepth*10;
	}
	else if(_userprv->m_usSampleWay == 0x03)
	{
		_userprv->m_fdeepth = _userprv->m_uiBMQSTAVal/100.0;
		_userprv->m_tSystemState.depth = _userprv->m_fdeepth*10;
	}
	Set_Baud(	_userprv->m_ucBaud );                                                                      //设置波特率
  _userprv->m_usSendCmdVal = _userprv->m_usOneFramNUM;                                              
}
/**
  * @brief  写配置参数
  * @param  
  *              
  * @retval                                                                                                                    
  */
 unsigned char databuff[30] = {0x00};//{0x36,0x37,0x38,0x39,0x13,0x40,0x10,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x53,0x54};
void Write_MoRen_Val(USERPRV * _userprv)//
{
	databuff[0] = (_userprv->m_uiBMQSTAVal>>24)&0xff;
	databuff[1] = (_userprv->m_uiBMQSTAVal>>16)&0xff;
	databuff[2] = (_userprv->m_uiBMQSTAVal>>8)&0xff;
	databuff[3] = (_userprv->m_uiBMQSTAVal)&0xff;
	//AT24CXX_Write(0x04,databuff,4);
	//第二页
	databuff[4] = (_userprv->m_usSampleSpan>>24)&0xff;
	databuff[5] = (_userprv->m_usSampleSpan>>16)&0xff; 
	databuff[6] = (_userprv->m_usSampleSpan>>8)&0xff;
	databuff[7] = (_userprv->m_usSampleSpan)&0xff;
	//AT24CXX_Write(8,databuff,4);
	databuff[8] = (_userprv->m_tModelDes.ID>>24)&0xff;
	databuff[9] = (_userprv->m_tModelDes.ID>>16)&0xff;
	databuff[10] = (_userprv->m_tModelDes.ID>>8)&0xff;
	databuff[11] = (_userprv->m_tModelDes.ID)&0xff;
	//AT24CXX_Write(12,databuff,4);
	//第三页
	databuff[12] = (_userprv->m_usOneFramNUM>>8)&0xff;
	databuff[13] = (_userprv->m_usOneFramNUM)&0xff;
	//AT24CXX_Write(16,databuff,2);
	databuff[14] = (_userprv->m_usOneMeterPaus>>8)&0xff;
	databuff[15] = (_userprv->m_usOneMeterPaus)&0xff;
	//AT24CXX_Write(18,databuff,2);
	databuff[16] = (_userprv->m_ucBaud)&0xff;
	//AT24CXX_Write(20,databuff,1);
	databuff[17] = (_userprv->m_usSampleWay)&0xff;
	//AT24CXX_Write(21,databuff,1);
	databuff[18] = (_userprv->m_ucMoNiQiDirec)&0xff;
	databuff[19] = _userprv->m_ucCommendFlag;
	
	databuff[20] = (_userprv->m_usMoNiQiSpeed>>8)&0xff;
	databuff[21] = _userprv->m_usMoNiQiSpeed&0xff;
	databuff[22] = _userprv->m_ucWaitTime;
	databuff[23] = _userprv->m_ucWaitTime1;
	AT24CXX_Write(0x04,databuff,24); 
}

/**
  * @brief  获取编码器的值
  * @param  
  *              
  * @retval 
  */
static  int delttimerval = 0;
void Get_BMQValue(USERPRV * _userprv)
{
	static uint16_t temp1 = 0;
	m_userprv.m_iDeltTimerCount = m_userprv.m_iTimer4UpdateCnt  - m_userprv.m_iTimerCount;   
	if(m_userprv.m_iDeltTimerCount <0)
	{
		m_userprv.m_uiBMQVal-= m_userprv.m_iTimerVal;
		m_userprv.m_uiBMQVal+= htim4.Instance->CNT;
		m_userprv.m_uiBMQVal+=65535*m_userprv.m_iDeltTimerCount;
		m_userprv.m_iTimerCount = m_userprv.m_iTimer4UpdateCnt ;
	  m_userprv.m_iTimerVal = htim4.Instance->CNT;
	}
	else if(m_userprv.m_iDeltTimerCount>0)
	{
		temp1 = 65535-(unsigned short)m_userprv.m_iTimerVal;
		m_userprv.m_uiBMQVal +=temp1;
		m_userprv.m_uiBMQVal += htim4.Instance->CNT;
		m_userprv.m_iDeltTimerCount-=1;
		m_userprv.m_uiBMQVal+=65535*m_userprv.m_iDeltTimerCount;
	  m_userprv.m_iTimerCount	 = m_userprv.m_iTimer4UpdateCnt ;
	  m_userprv.m_iTimerVal = htim4.Instance->CNT;
	}
	else
	{
		m_userprv.m_iDeltTimerCount = htim4.Instance->CNT -m_userprv.m_iTimerVal;
		m_userprv.m_uiBMQVal +=m_userprv.m_iDeltTimerCount;
		m_userprv.m_iTimerVal = htim4.Instance->CNT;
	}
}
/**
  * @brief  判断是否是A型模块
  * @param  
  *              
  * @retval 
  */
void Judge_A_Moudle(USERPRV * _userprv)
{
	if((!HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_2))&&(!HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_3)))
	{
		_userprv->m_tModelDes.type = 0x2;
	}
	else
		_userprv->m_tModelDes.type = 0x3;
}



/**
  * @brief  分析通用命令
  * @param  
  * 
  * @retval 
  */
void _analyse_(uint8_t * pdata,USERPRV * _userprv)
{
	uint16_t cmd1 = 0,cmd2 = 0,cmd3 = 0,cmd4 = 0,cmd5 = 0,cmd6 = 0,cmd7 = 0,cmd8 = 0,cmd9 = 0,cmd10 = 0;
	uint16_t data1 = 0,data2 = 0;
	cmd1 = (pdata[5]<<8)+pdata[6];
	cmd2 = (pdata[9]<<8)+pdata[10];
	cmd3 = (pdata[13]<<8)+pdata[14];
	cmd4 = (pdata[17]<<8)+pdata[18];
	cmd5 = (pdata[21]<<8)+pdata[22];
	cmd6 = (pdata[27]<<8)+pdata[28];
	cmd7 = (pdata[33]<<8)+pdata[34];
	cmd8 = (pdata[37]<<8)+pdata[38];
	cmd9 = (pdata[41]<<8)+pdata[42];
	cmd10 = (pdata[45]<<8)+pdata[46];
	if(cmd1 == 0x0012)//波特率
	{
	  data1 = (pdata[7]<<2)+(pdata[8])+1;
		_userprv->m_ucBaud = (uint8_t)data1;
		Set_Baud(data1);
	}
  if(cmd2 == 0x0002)//每米脉冲数
	{
		_userprv->m_usOneMeterPaus = (pdata[11]<<8)+(pdata[12]);
	}
	if(cmd3 == 0x0003)//每帧数据个数 
	{
		 _userprv->m_usOneFramNUM = (pdata[15]<<8)+(pdata[16]);
		 _userprv->m_usSendCmdVal = _userprv->m_usOneFramNUM;
	}
  if(cmd4 == 0x0004)//采样方式  发过来 是 0 1 2 而用的case 是 1 2 3
	{
		_userprv->m_usSampleWay = (pdata[19]<<8)+(pdata[20])+1;		
	}
	if(cmd5 == 0x0005)//采样间隔
	{
		_userprv->m_usSampleSpan = (pdata[23]<<24)+(pdata[24]<<16)+(pdata[25]<<8)+(pdata[26]);		
	}
	if(cmd6 == 0x0006)
	{
		 _userprv->m_uiBMQSTAVal = (pdata[29]<<24)+(pdata[30]<<16)+(pdata[31]<<8)+(pdata[32]);
     _userprv->m_fdeepth  = _userprv->m_uiBMQSTAVal*1.0;
		if(_userprv->m_usSampleWay == 2)
		{
			_userprv->m_uiBMQVal = (uint32_t)_userprv->m_fdeepth*_userprv->m_usOneMeterPaus;
		}
	}
	if(cmd8 == 0x0008)//模拟器速度
	{
		_userprv->m_usMoNiQiSpeed = (pdata[39]<<8)+(pdata[40]);		
	}
	if(cmd9 == 0x0009)// 模拟器方向
	{
		_userprv->m_ucMoNiQiDirec = (pdata[43]<<8)+(pdata[44])+1;		
	}
 if(cmd10 == 0x0016)//等待时间 1 每帧之间的间隔；等待时间 2；
	{
		//_userprv->m_ucMoNiQiDirec = (pdata[43]<<8)+(pdata[44])+1;		
		_userprv->m_ucWaitTime = pdata[47];
	  _userprv->m_ucWaitTime1 = pdata[48];
	}
}

/**
  * @brief  分析配置命令
  * @param  
  *              
  * @retval 
  */
void Analyse_Common_cmd(uint8_t * pdata,uint16_t cmdtype,USERPRV * _userprv)
{
	char buff[20];
	uint8_t speed = 0;
	float fspeed = 0,deepth = 0;
	switch (cmdtype)
	{
		case 0x0010://切换AB 相目前没有用
			_userprv->m_ucChangeAB =~_userprv->m_ucChangeAB;
		  break;
		case 0x0011:
	//液晶屏提示进入通用配置命令 模式
		  _userprv->m_ucMainPageFlag = 1;
	    Lcd_Clear(YELLOW);
	    Gui_DrawFont_GBK24(5,5,BLACK,YELLOW,"通用命令配置");
			break;
		case 0x0012://配置参数已经收到 进行解析存储
      _userprv->m_ucMainPageFlag = 0;
		  _userprv->m_uiMoniqicnt = 0;
			_analyse_(pdata,_userprv);
			_userprv->m_ucCommendFlag |=0x20;
      _userprv->m_ucWriteMoRenFlag = 1;
			Write_MoRen_Val(_userprv);
	    Lcd_Clear(BLUE);
	  	_userprv->musMONIQICNT = _userprv->m_usSampleSpan/_userprv->m_usMoNiQiSpeed*60;
	    Gui_DrawFont_GBK24(50,5,RED,BLUE,"暂停！！");
		  Gui_DrawFont_GBK16(5,40,RED,BLUE,"采样方式");
	    Gui_DrawFont_GBK16(5,60,RED,BLUE,"采样间");
			switch(_userprv->m_usSampleWay)
			{
				case 1:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"隔");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*100000;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"秒");
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					_userprv->m_fspeed = 0.1/(float)_userprv->m_usSampleSpan*3600;
					sprintf(buff,"%.2f",_userprv->m_fspeed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
					break;
				case 2:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*_userprv->m_usOneMeterPaus;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
				
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					sprintf(buff,"%.2f",speed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);	
					break;
				case 3:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->musMONIQICNT = _userprv->m_usSampleSpan*3600.0/_userprv->m_usMoNiQiSpeed;
					_userprv->m_fMONIQIdeepth = _userprv->m_fdeepth *1.0;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
					//_userprv->m_fdeepth = _userprv->m_fMONIQIdeepth*1.0;
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					_userprv->m_fspeed = _userprv->m_usMoNiQiSpeed;
					sprintf(buff,"%.2f",_userprv->m_fspeed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
					break;
			}
		  Gui_DrawFont_GBK16(5,80,RED,BLUE,"深度   :");
		  Gui_DrawFont_GBK16(5,100,RED,BLUE,"速度   :");
		  Gui_DrawFont_GBK16(200,80,RED,BLUE,"米");
		  Gui_DrawFont_GBK16(176,100,RED,BLUE,"米/时");
		  if(_userprv->m_usSampleWay == 0x01)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"等时间");
			}
			else if(_userprv->m_usSampleWay == 0x02)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"光电编码器");
			}
			else
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"模拟器");
			break;
		case 0x0014:  //上位机发送了 默认命令参数 读取 AT24CXX 默认值。
		  _userprv->m_ucMainPageFlag = 0;
		  _userprv->m_uiMoniqicnt = 0;
			Read_MoRen_Val(_userprv);
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
	    _userprv->m_ucReadMoRenFlag = 1;
		  Lcd_Clear(BLUE);
	    Gui_DrawFont_GBK24(50,5,RED,BLUE,"暂停！！");
		  Gui_DrawFont_GBK16(5,40,RED,BLUE,"采样方式");
		  Gui_DrawFont_GBK16(5,60,RED,BLUE,"采样间");
			switch(_userprv->m_usSampleWay)
			{
				case 1:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"隔");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*100000;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"秒");
					sprintf(buff,"%.2f",_userprv->m_usSampleSpan);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					_userprv->m_fspeed = 0.1/(float)_userprv->m_usSampleSpan*3600;
					sprintf(buff,"%.2f", _userprv->m_fspeed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
					break;
				case 2:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*_userprv->m_usOneMeterPaus;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
					_userprv->m_fdeepth =(float)m_userprv.m_uiBMQVal/(float)m_userprv.m_usOneMeterPaus;
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					sprintf(buff,"%.2f",speed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);	
				case 3:
					Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
					Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
					_userprv->musMONIQICNT = _userprv->m_usSampleSpan*3600.0/_userprv->m_usMoNiQiSpeed;
					Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
					sprintf(buff,"%.2f", _userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					_userprv->m_fspeed = _userprv->m_usMoNiQiSpeed*1.0;
					sprintf(buff,"%.2f",_userprv->m_fspeed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
					break;
			}
      Gui_DrawFont_GBK16(5,80,RED,BLUE,"深度    :");
		  Gui_DrawFont_GBK16(5,100,RED,BLUE,"速度    :");
		  Gui_DrawFont_GBK16(200,80,RED,BLUE,"米");
		  Gui_DrawFont_GBK16(176,100,RED,BLUE,"米/时");
		  if(_userprv->m_usSampleWay == 0x1)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"等时间");
			}
			else if(_userprv->m_usSampleWay == 0x2)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"光电编码器");
			}
			else if(_userprv->m_usSampleWay == 0x3)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"模拟器");
			}
			break;
		default :
			break;
	}
	cmdtype = 0xffff;
}

/**
  * @brief  Analyse_3005_CMD
  * @param  
  *              
  * @retval 
  */
static void Analyse_3005_CMD(uint16_t NUM,uint16_t startdata,uint16_t cmd,USERPRV * _userprv)
{
	uint16_t temp = 0;
	uint16_t temp_val = 0;
	uint16_t tempcout  = 0;
	uint16_t i = 0;
	uint16_t error_num_cout = 0;
	uint16_t right_num_cout = 0;
	uint8_t Copy_TransDataON = _userprv->m_ucTransDataON;
	_userprv->m_ucTransDataON = 3;
	temp = startdata;
 for(i = 0;i<NUM;i++)
	{
		_userprv->m_ucRExitflag = 0;
		Send_Data_To_FPGA_DoubleCmd(cmd,temp);
		while(!_userprv->m_ucRExitflag)
		{
			tempcout++;
			if(tempcout>100)
			{
				tempcout =0;
				break;
			}	
			Delay_ms(1);						
		}
		if(_userprv->m_ucRExitflag)
		{
			tempcout = 0;
			Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,0);
			temp_val = 0x00000000;
			temp_val = ((_userprv->m_usFPGARXbuff[0]>>8)&0xff)+((_userprv->m_usFPGARXbuff[0]&0xff)<<8); 
			if(temp_val == temp)
			{
				right_num_cout++;
			}
			else
			{
				error_num_cout++;
			}
			//TCP_server_out((uint8_t *)&_userprv->m_tInstrument,12);
		}
		else{
			tempcout = 0;
			error_num_cout++;
			//TCP_server_out((uint8_t *)&_userprv->m_tInstrument,12);
	 }
	 temp+=1;
	 Delay_ms(m_userprv.m_ucWaitTime1);	 
	} 
	_userprv->m_tInstrument_two.ALLNUM = NUM;
	_userprv->m_tInstrument_two.ERRORNUM = error_num_cout;
	_userprv->m_tInstrument_two.RIGHTNUM = right_num_cout;
	TCP_server_out((uint8_t *)&_userprv->m_tInstrument_two,16);
	_userprv->m_ucTransDataON = Copy_TransDataON;
}

/**
  * @brief  分析控制命令
  * @param  
  *              
  * @retval 
  */
void Analyse_control_cmd(uint8_t * pdata,uint16_t cmdtype,USERPRV * _userprv)
{
	static uint16_t data = 0,data1 = 0,data2 = 0,temp = 0,i = 0,j = 0;
	uint16_t tempcout  = 0,freecmdCount = 0;
	static uint32_t Depth = 0;
	unsigned char  buff[50];
	unsigned short cmd = 0,val = 0;
	switch (cmdtype)
	{
		case 0x3002: // 0x3002 可以忽略  最开始一般上位机 用到这条命令 后来 就删了 。
			data = (pdata[7]<<8) + pdata[8];
		  data1 = (pdata[9]<<8)+pdata[10];
		  Send_Data_To_FPGA_DoubleCmd(data,data1);
			//else {} //这里为什么不加上代码讷 应为 逻辑上这各else 语句是不会执行的。程序是按顺序执行的。 所以上面的 if 判断也不需要。	
			break;
		case 0x3003://上位机控制数据传输开关按钮命令
		  data = (pdata[7]<<8)+(pdata[8]);
		   switch (data)
			 {
				 case 0x0001:
					 _userprv->m_ucTransDataON = 1;		
           _userprv->m_tLoggingStaHand.timespan  = 0;
				   _userprv->m_uiMoniqicnt = 0;
  				 _userprv->m_uiTime = 0;
					 break;
				 case 0x0002:
					 _userprv->m_ucTransDataON = 0;
					 break;
			 }
			break;
		case 0x3004:  //仪器检测模式1
      data = (pdata[7]<<8)+(pdata[8]); 
			data1 = (pdata[9]<<8)+(pdata[10]);
		 _userprv->m_ucSlefTestON = 1;
			Send_Data_To_FPGA_DoubleCmd(data,data1);
			_userprv->m_ucRExitflag = 0;
			while(!_userprv->m_ucRExitflag)
			{
				tempcout++;
				Delay_ms(1);
				if(tempcout>20)
				{
					tempcout =0;
					break;
				}			
			}
			if(_userprv->m_ucRExitflag)
			{
				tempcout = 0;
				Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,0);
			  _userprv->m_tInstrument.data  = ((_userprv->m_usFPGARXbuff[0]>>8)&0xff)+((_userprv->m_usFPGARXbuff[0]&0xff)<<8); 
			  TCP_server_out((uint8_t *)&_userprv->m_tInstrument,12);
			}
		 data = 0;
			break;       //仪器检测模式2
			case 0x3005://这条命令收到后 407 自动累加发送的参数 。先把 个数提取出来 然后 发送一个数据FPGA 就弹回一个数据，直到所有的数据都 弹回来 这条命令才算完成。这里给FPGA  就 用3000 来�.�
			  _userprv->m_ucSlefTestON = 1;
			  data = (pdata[7]<<8)+(pdata[8]);
			  data1 = (pdata[9]<<8)+(pdata[10]);
			  data2 = (pdata[11]<<8)+(pdata[12]);
			  temp = data1;
			  Analyse_3005_CMD(data1,data2,data,_userprv);
//			 for(i = 0;i<data;i++)
//				{
//					_userprv->m_ucRExitflag = 0;
//		      Send_Data_To_FPGA_DoubleCmd(0X3000,temp);
//					while(!_userprv->m_ucRExitflag)
//					{
//						tempcout++;
//						if(tempcout>100)
//						{
//							tempcout =0;
//							break;
//						}	
//            Delay_ms(1);						
//					}
//					if(_userprv->m_ucRExitflag)
//					{
//						tempcout = 0;
//						Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,0);
//						_userprv->m_tInstrument.data =0x00000000;
//						_userprv->m_tInstrument.data  = ((_userprv->m_usFPGARXbuff[0]>>8)&0xff)+((_userprv->m_usFPGARXbuff[0]&0xff)<<8); 
//						TCP_server_out((uint8_t *)&_userprv->m_tInstrument,12);
//					}
//					else{
//						tempcout = 0;
//						_userprv->m_tInstrument.data =0x00000000;
//						TCP_server_out((uint8_t *)&_userprv->m_tInstrument,12);
//					}
//				 temp+=1;
//				 Delay_ms(m_userprv.m_ucWaitTime1);	 
//				} 
			  break;
		  case 0x3006://自由命令
			////双字命令 返回啥怎么返回需要用示波器看
			  buff[0] = 0xfd; buff[1] = 0x97; buff[2] = 0xff; buff[3] = 0xff;
			  buff[4] = 0x00; buff[5] = 0x00; buff[6] = 0x05; buff[7] = 0x30;
		    cmd = (pdata[7]<<8)+(pdata[8]);
		    val = (pdata[9]<<8)+(pdata[10]);
			  _userprv->m_ucRExitflag = 0;
			  Send_Data_To_FPGA_DoubleCmd(cmd,val);
			 	_userprv->m_ucALLDataReturnFlag = 0;
				while(1)
					{
						if(_userprv->m_ucRExitflag) 
						{
							_userprv->m_ucRExitflag = 0;
							Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,freecmdCount);
							freecmdCount++;
						}
						if(_userprv->m_ucALLDataReturnFlag>10)//100ms 没有返回数就认为出错了。
						{
							_userprv->m_ucALLDataReturnFlag = 0;
							break;
						}
					}
				for(i = 0;i<freecmdCount;i++)
					{
						
						buff[8+j] = (_userprv->m_usFPGARXbuff[i]>>8)&0xff;
						buff[9+j] = (_userprv->m_usFPGARXbuff[i])&0xff;
						j+=2;
					}  
			  buff[4] = (freecmdCount>>8)&0xff;
			  buff[5] = (freecmdCount)&0xff;
				TCP_server_out(buff,50);
			  i = 0;
				j = 0;
				freecmdCount = 0;
			  _userprv->m_ucALLDataReturnFlag = 0;
			  break;
			case 0x3007:  //置深命令
				Depth =  (pdata[7]<<24)+(pdata[8]<<16)+(pdata[9]<<8)+(pdata[10]);		
			  m_userprv.m_uiBMQVal = Depth*m_userprv.m_usOneMeterPaus;
				break;
			case 0x3008:  //单字命令
				data = (pdata[7]<<8)+(pdata[8]);  
				Send_Data_To_FPGA_OneCmd(data);
			break;
			case 0x3009:
				cmd = (pdata[8]<<8)+(pdata[9]);
		    val = (pdata[10]<<8)+(pdata[11]);
				switch(pdata[7])
				{
					case 1:// 钆示踪 同位素能谱模式
						Send_Data_To_FPGA_OneCmd(cmd);
						break;
					case 2://钆示     其他模式
						Send_Data_To_FPGA_DoubleCmd(cmd,val);
						break;
				}
			break;
	}
}

/**
  * @brief  分析设置IP参数命令
  * @param  
  *   
  * @retval 
  */

void Set_IP(USERPRV * _userprv,uint8_t * databuff)
{
  uint16_t cmd = (databuff[5]<<8)+databuff[6];
	switch (cmd)
	{
		case 0x1000:
			Gui_DrawFont_GBK16(5,35,RED,GRAY1,"模块  IP: ");
		  _userprv->m_tIPinformation.IP.data[3] = databuff[7];
      _userprv->m_tIPinformation.IP.data[2] = databuff[8];
      _userprv->m_tIPinformation.IP.data[1] = databuff[9];
      _userprv->m_tIPinformation.IP.data[0] = databuff[10];	
			Show_One_Line_Parameter(80,35,_userprv->m_tIPinformation.IP.data[3],
																	 _userprv->m_tIPinformation.IP.data[2],
																	 _userprv->m_tIPinformation.IP.data[1],
																	 _userprv->m_tIPinformation.IP.data[0],RED,GRAY1);		
			break;
		case 0x1001:
      Gui_DrawFont_GBK16(5,55,RED,GRAY1,"子网掩码: ");
			_userprv->m_tIPinformation.SUBNET.data[3] = databuff[7];
      _userprv->m_tIPinformation.SUBNET.data[2] = databuff[8];
      _userprv->m_tIPinformation.SUBNET.data[1] = databuff[9];
      _userprv->m_tIPinformation.SUBNET.data[0] = databuff[10];
			Show_One_Line_Parameter(80,55,_userprv->m_tIPinformation.SUBNET.data[3],
																	 _userprv->m_tIPinformation.SUBNET.data[2],
																	 _userprv->m_tIPinformation.SUBNET.data[1],
																	 _userprv->m_tIPinformation.SUBNET.data[0],RED,GRAY1);		
		
			break;
		case 0x1002:
			Gui_DrawFont_GBK16(5,75,RED,GRAY1,"默认网关: ");
			_userprv->m_tIPinformation.NETWORK.data[3] = databuff[7];
      _userprv->m_tIPinformation.NETWORK.data[2] = databuff[8];
      _userprv->m_tIPinformation.NETWORK.data[1] = databuff[9];
      _userprv->m_tIPinformation.NETWORK.data[0] = databuff[10];
			Show_One_Line_Parameter(80,75,_userprv->m_tIPinformation.NETWORK.data[3],
																	 _userprv->m_tIPinformation.NETWORK.data[2],
																	 _userprv->m_tIPinformation.NETWORK.data[1],
																	 _userprv->m_tIPinformation.NETWORK.data[0],RED,GRAY1);		
			break;
		case 0x1003:
			Gui_DrawFont_GBK16(5,95,RED,GRAY1,"服务器IP: ");
			Gui_DrawFont_GBK16(5,115,RED,GRAY1,"端口");
			_userprv->m_tIPinformation.Severs.data[3] = databuff[7];
      _userprv->m_tIPinformation.Severs.data[2] = databuff[8];
      _userprv->m_tIPinformation.Severs.data[1] = databuff[9];
      _userprv->m_tIPinformation.Severs.data[0] = databuff[10];	
			Show_One_Line_Parameter(80,95,_userprv->m_tIPinformation.Severs.data[3],
																	 _userprv->m_tIPinformation.Severs.data[2],
																	 _userprv->m_tIPinformation.Severs.data[1],
																	 _userprv->m_tIPinformation.Severs.data[0],RED,GRAY1);		
			Show_Num(80,115,RED,GRAY1,_userprv->m_tIPinformation.port);	
			break;
		case 0x1004:
			Gui_DrawFont_GBK16(5,135,RED,GRAY1,"DNS 地址: ");
			_userprv->m_tIPinformation.DNS.data[3] = databuff[7];
      _userprv->m_tIPinformation.DNS.data[2] = databuff[8];
      _userprv->m_tIPinformation.DNS.data[1] = databuff[9];
      _userprv->m_tIPinformation.DNS.data[0] = databuff[10];
			Show_One_Line_Parameter(80,135,_userprv->m_tIPinformation.DNS.data[3],
																	 _userprv->m_tIPinformation.DNS.data[2],
																	 _userprv->m_tIPinformation.DNS.data[1],
																	 _userprv->m_tIPinformation.DNS.data[0],RED,GRAY1);	
			break;
		case 0x1006:
		  Lcd_Clear(GRAY2);
	    Gui_DrawFont_GBK24(50,50,RED,GRAY2,"配置完成");
      Gui_DrawFont_GBK24(10,100,RED,GRAY2,"请断电重启设备！");
			Write_FLASH(_userprv);
      Show_One_Minate(_userprv,5);
      _userprv->m_usConfigIPFlag = 0x1006;
			break;
	}
}

/**
  * @brief  分析设置IP参数命令
  * @param  
  *              
  * @retval 
  */
void Analyse_SetIP_cmd(USERPRV * _userprv,uint8_t * databuff)
{
	//液晶屏提示进入设置IP 模式
	Lcd_Clear(GRAY1);
	Gui_DrawFont_GBK24(5,5,RED,GRAY1,"进入配置IP模式");
	_userprv->m_usConfigIPFlag = (databuff[5]<<8) + (databuff[6]);	
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
	while(1)
	{
		Set_IP(_userprv,_userprv->m_ucETHRXbuff);
		Delay_ms(500);
	}
}
/**
  * @brief  分析ETH 或者是USB命令
  * @param  
  *              
  * @retval 
  */
void Analyse_EHT_USB_CMD(USERPRV * _userprv)
{
	uint16_t cmdtype = 0;
	if(_userprv->m_ucETHRXFLAG||_userprv->m_ucUSBRXFlag)
	{
		if(_userprv->m_ucETHRXFLAG)
		 cmdtype = (_userprv->m_ucETHRXbuff[5]<<8)+(_userprv->m_ucETHRXbuff[6]);
		if((cmdtype>0)&&(cmdtype<0x0fff))
		{
			if(_userprv->m_ucETHRXFLAG)
			  Analyse_Common_cmd(_userprv->m_ucETHRXbuff,cmdtype,_userprv);
		}
		else if((cmdtype >= 0x1000)&&(cmdtype<0x01fff))  //设置IP 命令
		{
			if(_userprv->m_ucETHRXFLAG)
			  Analyse_SetIP_cmd(_userprv,_userprv->m_ucETHRXbuff);
		}
		else if((cmdtype >= 0x2000)&&(cmdtype<0x2fff))
		{
			
		}
		else if((cmdtype >= 0x3000)&&(cmdtype<0x03fff))   //控制命令
		{
			if(_userprv->m_ucETHRXFLAG)
			  Analyse_control_cmd(_userprv->m_ucETHRXbuff,cmdtype,_userprv);
		}
		else if((cmdtype >= 0x4000)&&(cmdtype<0x0ffff))//待定
		{
			
		}
	}
	_userprv->m_ucETHRXFLAG = 0;
	_userprv->m_ucUSBRXFlag = 0;
}

/**
  * @brief  发送数据包括测井状态数据 仪器检测状态数据
  * @param   
  *              
  * @retval 
  */
extern void Send_Fixed_Fpga_Data(uint16_t num,uint8_t * buff);
void Trans_Data(USERPRV * _userprv)
{
	uint16_t FPGAData = 0;
	float _fspeed = 0.0;
	static uint16_t num = 0;
	char buff[20];
	float a = 0.0,b = 0.0,_fDepth = 0.0;
	float _fTime;
  if((_userprv->m_ucTransDataON == 1)&&(	_userprv->m_ucMainPageFlag == 0))     //这个地方还要加上 停止FALG 
	{
		Gui_DrawFont_GBK24(50,5,RED,BLUE,"测井状态");
		switch (_userprv->m_usSampleWay)
		{
			case 1://时间方式 				
				_userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;
				if((_userprv->m_tLoggingStaHand.timespan-_userprv->m_uiTime)>_userprv->m_usSampleSpan_val)//定时间 间隔采集
				{
					Get_BMQValue(_userprv);
					_userprv->m_fdeepth -=0.1; 
					_fTime = htim5.Instance->CNT*1.0/100000.0;
					_userprv->m_tLoggingStaHand.depth = _userprv->m_fdeepth *10;//深度是根据光电编码 器获得  或者 模拟器获得 
					_userprv->m_tSystemState.depth = _userprv->m_tLoggingStaHand.depth;
					_userprv->m_tSystemState.time = _userprv->m_tLoggingStaHand.timespan;
					sprintf(buff,"%.2f",_userprv->m_fdeepth);
					Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
					sprintf(buff,"%.2f",_userprv->m_fspeed);
					Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);					
					if(_userprv->m_ucETHCommuFlag)//eth 通信
					TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaHand,sizeof(_userprv->m_tLoggingStaHand));
					else if(_userprv->m_ucUSBCommuFlag)//usb 通信
					USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,(uint8_t *)&_userprv->m_tLoggingStaHand,sizeof(_userprv->m_tLoggingStaHand));
					//正在发送数据 等待flag 置位
					_userprv->m_ucWaitReturnEnd = 1;
					//发送返回数据命令
					Send_Data_To_FPGA_OneCmd(_userprv->m_usSendCmdVal);//0x1000 0x1200
					_userprv->m_ucALLDataReturnFlag = 0;
					while(1)
					{
						if(_userprv->m_ucRExitflag)
						 {
								Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,num);		
								num++;
								_userprv->m_ucRExitflag = 0;
								_userprv->m_ucALLDataReturnFlag = 0;
							}
							if(num == 256)
							{
								_userprv->m_tLoggingStaEnd.num+=256; 
								TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,512);
								Delay_us(800);
								num = 0;
							 }
							else if(_userprv->m_ucALLDataReturnFlag>_userprv->m_ucWaitTime)
							{
								_userprv->m_ucALLDataReturnFlag = 0;
								if(num >=1)
									{
										TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,num*2);
										Delay_us(800);
										//Send_Fixed_Fpga_Data(num,(uint8_t *)&_userprv->m_usFPGARXbuff);
										_userprv->m_tLoggingStaEnd.num +=num;
										num = 0;
									}
								break;
							}
					}
					
					//将尾巴发送出去
					TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaEnd,sizeof(_userprv->m_tLoggingStaEnd));
					num = 0;
					//更新时间
					_userprv->m_uiTime = _userprv->m_tLoggingStaHand.timespan;
					//发送数据等待flag 清零
					_userprv->m_ucWaitReturnEnd = 0;
					_userprv->m_tLoggingStaEnd.num = 0;
				}
			break;
				
			case 2://脉冲方式
				 	Get_BMQValue(_userprv);	
					if(abs(_userprv->m_uiBMQVal-_userprv->m_uiBMQValLast)>_userprv->m_usSampleSpan_val)//定时间 间隔采集
					{
						_userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;//获取当前时间
						_userprv->m_fdeepth = (float)_userprv->m_uiBMQVal/(float)_userprv->m_usOneMeterPaus;
						_userprv->m_tLoggingStaHand.depth =   _userprv->m_fdeepth *10;//深度是根据光电编码 器获得  或者 模拟器获得
						_userprv->m_tSystemState.depth = _userprv->m_tLoggingStaHand.depth;
						_userprv->m_tSystemState.time = _userprv->m_tLoggingStaHand.timespan;	
						_fTime = htim5.Instance->CNT*1.0/100000.0;
						sprintf(buff,"%.2f", _userprv->m_fdeepth);
						Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
						_userprv->m_fspeed = (float)_userprv->m_usSampleSpan*360000000.0/abs(_userprv->m_tLoggingStaHand.timespan - _userprv->m_uiTime);
						sprintf(buff,"%.2f", _userprv->m_fspeed );
						Gui_DrawFont_GBK16(90,100,RED,BLUE,"          ");
						Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);	
						TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaHand,sizeof(_userprv->m_tLoggingStaHand));
						//正在发送数据 等待flag 置位
						_userprv->m_ucWaitReturnEnd = 1;
						//发送返回数据命令
					  Send_Data_To_FPGA_OneCmd(_userprv->m_usSendCmdVal);//0x1000 0x1200
						_userprv->m_ucALLDataReturnFlag = 0;
					  while(1)
						{
							if(_userprv->m_ucRExitflag)
							 {
									Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,num);		
									num++;
									_userprv->m_ucRExitflag = 0;
									_userprv->m_ucALLDataReturnFlag = 0;
								}
								if(num == 256)
								{
									_userprv->m_tLoggingStaEnd.num+=256; 
									TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,512);
									Delay_us(800);
									num = 0;
								 }
								else if(_userprv->m_ucALLDataReturnFlag>_userprv->m_ucWaitTime)
								{
									_userprv->m_ucALLDataReturnFlag = 0;
									if(num >=1)
										{
											TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,num*2);
											Delay_us(800);
											//Send_Fixed_Fpga_Data(num,(uint8_t *)&_userprv->m_usFPGARXbuff);
											_userprv->m_tLoggingStaEnd.num +=num;
											num = 0;
										}
									break;
								}
						}
						//将尾巴 发送回去
						TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaEnd,sizeof(_userprv->m_tLoggingStaEnd));
						_userprv->m_tLoggingStaEnd.num = 0;
						//更新脉冲计数
						_userprv->m_uiBMQValLast = _userprv->m_uiBMQVal;
						//发送数据等待flag 清零
						_userprv->m_ucWaitReturnEnd = 0;
						_userprv->m_tLoggingStaEnd.num  = 0;
				   _userprv->m_uiTime = _userprv->m_tLoggingStaHand.timespan;
					 _userprv->m_flastdeepth = _userprv->m_fdeepth ;	
					}					
				break;
				
			case 3://模拟器方式
          _userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;//获取当前时间
					if((_userprv->m_uiMoniqicnt-_userprv->m_uiMoniqicntlast)>	_userprv->musMONIQICNT)//定时间 间隔采集
					{
					  _userprv->m_fdeepth += (float)(_userprv->m_usMoNiQiSpeed)*(_userprv->m_uiMoniqicnt-_userprv->m_uiMoniqicntlast)/3600;
						_userprv->m_tLoggingStaHand.depth =   _userprv->m_fdeepth *10;
						sprintf(buff,"%.2f",_userprv->m_fdeepth);
						Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
						sprintf(buff,"%.2f",_userprv->m_fspeed);
						Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
						if(_userprv->m_ucETHCommuFlag)//eth 通信
						  TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaHand,sizeof(_userprv->m_tLoggingStaHand));
						else if(_userprv->m_ucUSBCommuFlag)//usb 通信
						  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,(uint8_t *)&_userprv->m_tLoggingStaHand,sizeof(_userprv->m_tLoggingStaHand));
						//正在发送数据 等待flag 置位
						_userprv->m_ucWaitReturnEnd = 1;
						//发送返回数据命令
					  Send_Data_To_FPGA_OneCmd(_userprv->m_usSendCmdVal);//0x1000 0x1200
						_userprv->m_ucALLDataReturnFlag = 0;
						while(1)
						{
							if(_userprv->m_ucRExitflag)
							 {
									Read_Data_From_FPGA(_userprv->m_usFPGARXbuff,num);		
									num++;
									_userprv->m_ucRExitflag = 0;
									_userprv->m_ucALLDataReturnFlag = 0;
								}
								if(num == 256)
								{
									_userprv->m_tLoggingStaEnd.num+=256; 
									TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,512);
	                Delay_us(800);
									num = 0;
								 }
								else if(_userprv->m_ucALLDataReturnFlag>_userprv->m_ucWaitTime)
								{
									_userprv->m_ucALLDataReturnFlag = 0;
									if(num >=1)
										{
										  TCP_server_out((uint8_t *)&_userprv->m_usFPGARXbuff,num*2);
	                    Delay_us(800);
											//Send_Fixed_Fpga_Data(num,(uint8_t *)&_userprv->m_usFPGARXbuff);
											_userprv->m_tLoggingStaEnd.num +=num;
											num = 0;
										}
									break;
								}
						}
						//将尾巴 发送回
						TCP_server_out((uint8_t *)&_userprv->m_tLoggingStaEnd,sizeof(_userprv->m_tLoggingStaEnd));
						//更新脉冲计数
            _userprv->m_uiMoniqicntlast = _userprv->m_uiMoniqicnt ;
						//发送数据等待flag 清零
						_userprv->m_ucWaitReturnEnd = 0;
						_userprv->m_tLoggingStaEnd.num  = 0;
			     _userprv->m_uiMoniqicntlast = _userprv->m_uiMoniqicnt;
					}									
				break;
			default :
				Delay_ms(1);
				break;
		}
	}
	else if((_userprv->m_ucTransDataON == 0)&&(_userprv->m_ucMainPageFlag == 0))  //空闲状态
	{
		_userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;
	  Gui_DrawFont_GBK24(50,5,RED,BLUE,"暂停！！");
		switch(_userprv->m_usSampleWay)
		{
			case 1:
	      _userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;
				_fTime = htim5.Instance->CNT*1.0/100000.0;
				_userprv->m_tLoggingStaHand.depth = _userprv->m_fdeepth*10;//深度是根据光电编码 器获得  或者 模拟器获得 
				_userprv->m_tSystemState.depth = _userprv->m_tLoggingStaHand.depth;
				_userprv->m_tSystemState.time = _userprv->m_tLoggingStaHand.timespan;
				sprintf(buff,"%.2f",_userprv->m_fdeepth);
				Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
				sprintf(buff,"%.2f",_userprv->m_fspeed);
				Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);				
			break;
			case 2:
			 	Get_BMQValue(_userprv);
				_userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;//获取当前时间
	      _userprv->m_fdeepth = (float)_userprv->m_uiBMQVal/(float)_userprv->m_usOneMeterPaus;
				_userprv->m_tLoggingStaHand.depth =   _userprv->m_fdeepth *10;//深度是根据光电编码 器获得  或者 模拟器获得
				_userprv->m_tSystemState.depth = _userprv->m_tLoggingStaHand.depth;
				_userprv->m_tSystemState.time = _userprv->m_tLoggingStaHand.timespan;			
		  	sprintf(buff,"%.2f",_userprv->m_fdeepth );
				Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
				sprintf(buff,"%.2f", _userprv->m_fspeed);
			  if(_userprv->m_fspeed == 0.00)
				{
					Gui_DrawFont_GBK16(90,100,RED,BLUE,"          ");
					_userprv->m_fspeed += 0.01;
				}
  			Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
			  _userprv->m_uiTime = _userprv->m_tLoggingStaHand.timespan;	
				break;
			case 3:
				_userprv->m_tLoggingStaHand.timespan = htim5.Instance->CNT;//获取当前时间
				_userprv->m_fdeepth += (float)(_userprv->m_fspeed)*(_userprv->m_uiMoniqicnt-_userprv->m_uiMoniqicntlast)/3600;
			  m_userprv.m_tSystemState.depth = _userprv->m_fdeepth*10;
				sprintf(buff,"%.2f",_userprv->m_fdeepth);
				Gui_DrawFont_GBK16(90,80,RED,BLUE,buff);
				sprintf(buff,"%.2f",_userprv->m_fspeed);
				Gui_DrawFont_GBK16(90,100,RED,BLUE,buff);
				_userprv->m_uiMoniqicntlast = _userprv->m_uiMoniqicnt ;
				break;
		}	
	}
}

/**
  * @brief 伽马通信检测 中子通信检测 数据初始化
  * @param  
  *              
  * @retval 
  */
void Zhongzi_Yama_Data_Init(TEST_Y * test_y,TEST_Z *  test_z)
{
	uint32_t i = 0;
	uint8_t j = 0;
	uint8_t flag = 0;
	uint16_t temp = 0x094a;
	
	for(i = 0;i<DATACHECKLength;i++)
	{
		test_y[i].head = 0x3000;
		if(!flag)
		{
		  test_y[i].value = (temp<<j)&0xFfff;
		}
		else
		  test_y[i].value = (temp>>j)&0xFfff;	
		if(j==12)
		{
			j = 0;
			flag = !flag;
		}
		j++;
	}
	j = 0;
	flag = 0;
	temp = 0x094a;
	for(i = 0;i<DATACHECKLength;i++)
	{
		test_z[i].head = 0x2000;
		if(!flag)
		{
		  test_z[i].value = (temp<<j)&0xFfff;
		}
		else
		  test_z[i].value = (temp>>j)&0xFfff;	
		if(j==12)
		{
			j = 0;
			flag = !flag;
		}
		j++;
	}
}

/**
  * @brief 伽马通信测试 中子通信测试 
  * @param  
  *              
  * @retval 
  */
uint8_t Z_Y_TestFunc(USERPRV * _userprv)
{
	uint8_t buff[20];
	uint8_t data,data1;
	uint8_t trans = 0;
  uint16_t i = 0;
	uint16_t  tempcout = 0;
	uint16_t tempval = 0;
	float errorcount = 0.0f;

	if(_userprv->m_ucYamaFlag)
	{
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
    Lcd_Clear(GREEN);
		Gui_DrawFont_GBK24(30,5,RED,GREEN,"伽马中子误码率");
		Gui_DrawFont_GBK24(5,40,RED,GREEN,"伽马");
    trans = _userprv->m_ucTransDataON;
		_userprv->m_ucTransDataON = 2;
		for(i = 0;i<DATACHECKLength;i++)
		{
			Send_Data_To_FPGA_DoubleCmd(_userprv->m_tTest_Y[i].head,_userprv->m_tTest_Y[i].value);
			_userprv->m_ucRExitflag = 0;
			while(!_userprv->m_ucRExitflag)
			{
				tempcout++;
				if(tempcout>20)
				{
					tempcout =0;
					break;
				}	
				Delay_ms(1);				
			}
			if(_userprv->m_ucRExitflag)
			{
				tempcout = 0;
				Read_Data_From_FPGA(&_userprv->m_usTestRxbuff[i],0);
				data = (uint8_t)(_userprv->m_usTestRxbuff[i]&0xff);
			  data1 = (uint8_t)(_userprv->m_usTestRxbuff[i]>>8);
			}
			tempval = (data<<8)+data1;
			if(tempval != _userprv->m_tTest_Y[i].value)
			{
				errorcount+=1;
			}
			tempval = 0XFFFF;
			Delay_ms(50);
    }
	  _userprv->m_fYamaError = errorcount/DATACHECKLength;
		Show_Num_float(5,70,RED,GREEN,_userprv->m_fYamaError);
		_userprv->m_ucTransDataON = 2;
	  _userprv->m_tYA_ZhongZi.allnum = DATACHECKLength;
		_userprv->m_tYA_ZhongZi.errornum = errorcount;
		_userprv->m_tYA_ZhongZi.Y_Z = 0x02;
		TCP_server_out((uint8_t *)&_userprv->m_tYA_ZhongZi,14);
		Show_One_Minate(_userprv,5);
		_userprv->m_ucTransDataON = trans;
		if(_userprv->m_ucMainPageFlag == 1)
		{
			Welcome_Page(_userprv);
		}
		else
		{
			Lcd_Clear(BLUE);
			Gui_DrawFont_GBK24(50,5,RED,BLUE,"暂停！！");
			Gui_DrawFont_GBK16(5,40,RED,BLUE,"采样方式");
			Gui_DrawFont_GBK16(5,60,RED,BLUE,"采样间");
			switch(_userprv->m_usSampleWay)
		{
			case 1:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"隔");
				Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*100000;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"秒");
				break;
			case 2:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
			  Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*_userprv->m_usOneMeterPaus;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
				break;
			case 3:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
			  Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->musMONIQICNT = _userprv->m_usSampleSpan/_userprv->m_usMoNiQiSpeed*60;
		    _userprv->m_fMONIQIdeepth = _userprv->m_usSampleSpan*1.0;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
				break;
		}
		  Gui_DrawFont_GBK16(5,80,RED,BLUE,"深度   :");
		  Gui_DrawFont_GBK16(5,100,RED,BLUE,"速度   :");
		  Gui_DrawFont_GBK16(200,80,RED,BLUE,"米");
		  Gui_DrawFont_GBK16(176,100,RED,BLUE,"米/时");
		  if(_userprv->m_usSampleWay == 0x01)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"等时间");
			}
			else if(_userprv->m_usSampleWay == 0x02)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"光电编码器");
			}
			else
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"模拟器");
		}
		_userprv->m_ucYamaFlag = 0;
	}
	//中子
	if(_userprv->m_ucZhongziFlag)
	{
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
		Lcd_Clear(GREEN);
		Gui_DrawFont_GBK24(30,5,RED,GREEN,"伽马中子误码率");
	  Gui_DrawFont_GBK24(5,40,RED,GREEN,"中子");
	  _userprv->m_ucTransDataON = 2;
		for(i = 0;i<DATACHECKLength;i++)
		{
			Send_Data_To_FPGA_DoubleCmd(_userprv->m_tTest_Z[i].head,_userprv->m_tTest_Z[i].value);
			_userprv->m_ucRExitflag = 0;
			while(!_userprv->m_ucRExitflag)
			{
				tempcout++;
				if(tempcout>20)
				{
					tempcout =0;
					break;
				}
				Delay_ms(1);					
			}
			if(_userprv->m_ucRExitflag)
			{
				tempcout = 0;
				Read_Data_From_FPGA(&_userprv->m_usTestRxbuff[i],0);
				data = (uint8_t)(_userprv->m_usTestRxbuff[i]&0xff);
			  data1 = (uint8_t)(_userprv->m_usTestRxbuff[i]>>8);
			  tempval = (data<<8)+data1;
			}			
			if(tempval != _userprv->m_tTest_Z[i].value)
			{
				errorcount+=1;
				
			}
				tempval = 0XFFFF;
				Delay_ms(50);
    }
			_userprv->m_fYamaError = errorcount/DATACHECKLength;
			Show_Num_float(5,70,RED,GREEN,_userprv->m_fYamaError);
			_userprv->m_ucTransDataON = 2;
			_userprv->m_tYA_ZhongZi.allnum = DATACHECKLength;
			_userprv->m_tYA_ZhongZi.errornum = errorcount;
			_userprv->m_tYA_ZhongZi.Y_Z = 0x01;
			TCP_server_out((uint8_t *)&_userprv->m_tYA_ZhongZi,14);
			Show_One_Minate(_userprv,5);
		if(_userprv->m_ucMainPageFlag == 1)
		{
			Welcome_Page(_userprv);
		}
		else
		{
			Lcd_Clear(BLUE);
			Gui_DrawFont_GBK24(50,5,RED,BLUE,"暂停！！");
			Gui_DrawFont_GBK16(5,40,RED,BLUE,"采样方式");
			Gui_DrawFont_GBK16(5,60,RED,BLUE,"采样间");
			switch(_userprv->m_usSampleWay)
		  {
			case 1:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"隔");
				Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*100000;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"秒");
				break;
			case 2:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
			  Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->m_usSampleSpan_val = _userprv->m_usSampleSpan*_userprv->m_usOneMeterPaus;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
				break;
			case 3:
				Gui_DrawFont_GBK16(54,60,RED,BLUE,"距");
			  Show_Num(90,60,RED,BLUE,_userprv->m_usSampleSpan);
			  _userprv->musMONIQICNT = _userprv->m_usSampleSpan/_userprv->m_usMoNiQiSpeed*60;
		    _userprv->m_fMONIQIdeepth = _userprv->m_usSampleSpan*1.0;
			  Gui_DrawFont_GBK16(200,60,RED,BLUE,"米");
				break;
		   }
		  Gui_DrawFont_GBK16(5,80,RED,BLUE,"深度   :");
		  Gui_DrawFont_GBK16(5,100,RED,BLUE,"速度   :");
		  Gui_DrawFont_GBK16(200,80,RED,BLUE,"米");
		  Gui_DrawFont_GBK16(176,100,RED,BLUE,"米/时");
		  if(_userprv->m_usSampleWay == 0x01)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"等时间");
			}
			else if(_userprv->m_usSampleWay == 0x02)
			{
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"光电编码器");
			}
			else
				Gui_DrawFont_GBK16(90,40,RED,BLUE,"模拟器");
		}
		_userprv->m_ucZhongziFlag = 0;		
	}
	return 1;
}	

/**
  * @brief  空函数 没有用
  * @param  
  *              
  * @retval 
  */
void 	Updata_Time_Depth(USERPRV * _userprv)
{

}
/**
  * @brief  处理 3004 自检  空函数 没有用
  * @param  
  *              
  * @retval 
  */
void DealWith_0x3004(USERPRV * _userprv)
{
	
}
/**
  * @brief  A 模块函数  
  * @param  
  *              
  * @retval 
  */
uint32_t temp = 0,temp1 = 0;
void A_Moudel_Function(USERPRV * _userprv)
{
  Analyse_EHT_USB_CMD(_userprv);//处理以太网下发的命令
	Trans_Data(_userprv);         //数据传输函数
	Z_Y_TestFunc(_userprv);       //处理中子伽马按键中断
//	Get_BMQValue(_userprv);
	if(_userprv->m_ucFLUSH_YJP)//刷新液晶屏
	{
		_userprv->m_ucFLUSH_YJP = 0;
		Welcome_Page(_userprv);
	}
	if((_userprv->m_ucOneScnt>100)&&(_userprv->m_ucMainPageFlag == 0))//每10秒存一次深度信息
	{
	 if(_userprv->m_usSampleWay == 0x01)
		{
			 temp= (uint32_t)(m_userprv.m_fdeepth*100);
			 AT24CXX_Write(0x00,(uint8_t *)&temp,4);
			 Delay_ms(1);
		}
		else if(_userprv->m_usSampleWay == 0x02)
		{
			 AT24CXX_Write(0x00,(uint8_t *)&m_userprv.m_uiBMQVal,4);
		}
		else if(_userprv->m_usSampleWay == 0x03)
		{
			 temp= (uint32_t)(m_userprv.m_fdeepth*100);
			 AT24CXX_Write(0x00,(uint8_t *)&temp,4);
			 AT24CXX_Read(0x00,(uint8_t *)temp1,4);
		}
		_userprv->m_ucOneScnt = 0;
	}
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
}

/**
  * @brief  A_B 模块函数
  * @param  
  *              
  * @retval 
  */
void A_B_Moudel_Function(USERPRV * _userprv)
{ 
	switch (_userprv->m_tModelDes.type)
	{
		case 0x2://A 型机
			A_Moudel_Function(_userprv);
			break;
		case 0x3://B型机
			
		  break;
		default:
			Delay_ms(1);
		  break;
	}
}
/**
  * @brief  判断AB 模块
  * @param  
  *              
  * @retval 
  */
void Judge_A_B_Moudel(USERPRV * _userprv)
{
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_3) == RESET)
	{
		_userprv->m_tModelDes.type = 0x02;
	}
	else
		_userprv->m_tModelDes.type = 0x3;
}
/**
  * @brief  开始欢迎界面
  * @param  
  *              
  * @retval 
  */
void Welcome_Page(USERPRV * _userprv)
{
	unsigned char buff[20];
	showimage(gImage_start,0,0);
	Gui_DrawFont_GBK24(50,80,RED,BLUE,"欢迎使用！");
	Gui_DrawFont_GBK16(5,120,RED,BLUE, "TYPE:");
	if(_userprv->m_tModelDes.type == 0x2)
    Gui_DrawFont_GBK16(48,120,RED,BLUE, "A");
	else
	  Gui_DrawFont_GBK16(48,120,RED,BLUE, "B");
	Gui_DrawFont_GBK16(5,150,RED,BLUE, "ID  :");
	sprintf(buff,"%x",_userprv->m_tModelDes.ID);
	Gui_DrawFont_GBK16(48,150,RED,BLUE,buff);
  //Show_Num(48,150,RED,BLUE,_userprv->m_tModelDes.ID);
	
}

/**
  * @brief  写EEPROM
  * @param  
  *              
  * @retval 
  */
extern I2C_HandleTypeDef hi2c1;
void Write_At24c02(USERPRV * _userprv)
{
	//if(HAL_I2C_Mem_Write(&hi2c1,AT24C0XXDEVICEADRR,0x04,8,(uint8_t *)&_userprv->m_uiBMQVal,4,1) == HAL_OK)
	//{
	//	Show_Num(48,150,RED,BLUE,1);
//	}
	//else
	//	Show_Num(48,150,RED,BLUE,2);
}
/**
  * @brief  读AT24C02
  * @param  
  *              
  * @retval 
  */
void Read_At24c02(USERPRV * _userprv)
{
	//if(HAL_I2C_Mem_Read(&hi2c1, AT24C0XXDEVICEADRR, 0x04, 8, (uint8_t *)&_userprv->m_uiBMQVal, 4, 1)==HAL_OK)
//	{
	//	Show_Num(48,150,RED,BLUE,3);
//	}
	//else
	//	Show_Num(48,150,RED,BLUE,4);
}

/**
  * @brief  发送 模块说明
  * @param  
  *              
  * @retval 
  */
void Send_Moudel(USERPRV * _userprv)
{
	TCP_server_out((unsigned char *)&_userprv->m_tModelDes,sizeof(_userprv->m_tModelDes));
}

