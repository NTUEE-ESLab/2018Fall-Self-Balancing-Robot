
#include "includes.h"

/***************************************************************
** ��������: Timer1_Update
** ��������: Timer1�жϺ���
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void Timer1_Update(void) interrupt  3
{
  
   	TL1 = 0xE5;		    //���س�ֵ ��ʱ10ms
   	TH1 = 0xBE;
   	
	SampleInputVoltage();	
   	AngleControl();	   		
   	SpeedControl();          
	MotorOutput();	  
		
#if 0  /*������*/  
   g_ucLEDCount++;
   if(g_ucLEDCount >=100) //LED1��1�뽻����˸
   {
   	 g_ucLEDCount=0;
	 LED3=~LED3;
   }
#endif
   
#if 0  /*������ Ԥ��������*/
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
** ��������: GetLeftMotorPulse
** ��������: �������岶׽����
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/

void GetLeftMotorPulse(void) interrupt 2
{
#if 0
   if(LeftMotorDirection == 1) 			//1 
   {
      g_liLeftMotorPulse++;  //A����͵�ƽ��B�����һ
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
** ��������: GetRightMotorPulse
** ��������: �ҵ�����岶׽����
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
/*
void GetRightMotorPulse(void) interrupt 0
{
#if 0
   if(RightMotorDirection == 0) 	   //0
   {
      g_liRightMotorPulse++; 	//A����͵�ƽ��B�����һ
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