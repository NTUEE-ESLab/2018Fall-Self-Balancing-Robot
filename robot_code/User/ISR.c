
#include "includes.h"

/***************************************************************
** 函数名称: Timer1_Update
** 功能描述: Timer1中断函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void Timer1_Update(void) interrupt  3
{
  
   	TL1 = 0xE5;		    //重载初值 定时10ms
   	TH1 = 0xBE;
   	
	SampleInputVoltage();	
   	AngleControl();	   		
   	SpeedControl();          
	MotorOutput();	  
		
#if 0  /*调试用*/  
   g_ucLEDCount++;
   if(g_ucLEDCount >=100) //LED1灯1秒交替闪烁
   {
   	 g_ucLEDCount=0;
	 LED3=~LED3;
   }
#endif
   
#if 0  /*调试用 预编译命令*/
   OutData[0] = g_fCarAngle;
   OutData[1] = g_fGravityAngle;
   OutData[2] = g_fGyroAngleSpeed ;
   OutData[3] = g_iAccelInputVoltage_Y_Axis;
   
   OutPut_Data();
#endif	
}

void Timer0_Update(void) interrupt  1
{
    
	g_ucSpeedCountCarry++;
	LED2=~LED2;
}



/***************************************************************
** 函数名称: GetLeftMotorPulse
** 功能描述: 左电机脉冲捕捉函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/

void GetLeftMotorPulse(void) interrupt 2
{
#if 0
   if(LeftMotorDirection == 1) 			//1 
   {
      g_liLeftMotorPulse++;  //A脉冲低电平则B脉冲加一
      //LED0=~LED0;
   }	 
   else 
   {
      g_liLeftMotorPulse--; 
      //LED0=~LED0;
   }
#else
	  g_liLeftMotorPulse++;
	  LED0=~LED0;
#endif	
} 


/***************************************************************
** 函数名称: GetRightMotorPulse
** 功能描述: 右电机脉冲捕捉函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
/*
void GetRightMotorPulse(void) interrupt 0
{
#if 0
   if(RightMotorDirection == 0) 	   //0
   {
      g_liRightMotorPulse++; 	//A脉冲低电平则B脉冲加一
   		//LED2=~LED2;
   }
   else 
   {
      g_liRightMotorPulse--;
		//LED2=~LED2;
   }
#else
		g_liRightMotorPulse++;
		//LED2=~LED2;
#endif
}
*/						 