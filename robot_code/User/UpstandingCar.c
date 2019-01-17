

#include "includes.h"

unsigned char g_ucLEDCount;
/******电机控制参数******/
float g_fSpeedControlOut;
float g_fAngleControlOut;
float g_fLeftMotorOut;
float g_fRightMotorOut;
/******角度控制参数******/
int   g_iAccelInputVoltage_Y_Axis ;	//加速度Y轴数据
int   g_iGyroInputVoltage_X_Axis  ;	//陀螺仪X轴数据
//long int xdata g_liAccAdd;
//long int xdata g_liGyroAdd;
float g_fCarAngle;         			//车模倾角
float g_fGyroAngleSpeed;			//角速度      			
float g_fGyroscopeAngleIntegral;	//角速度积分值
float g_fGravityAngle;				//加速度初步计算得到的倾角
/******速度控制参数******/
long int   g_liLeftMotorPulse;
long int   g_liRightMotorPulse;
float g_fCarSpeed;
float g_fCarSpeedOld;
float g_fCarPosition;
unsigned char g_ucSpeedCountCarry;
/*-----角度环和速度环PID控制参数-----*/
float code g_fcAngle_P = 12;	//12		调到能直立 数值大摆动大
float code g_fcAngle_D = 0.9;	//0.9 	 调大时左右振动 调到微动就好
float code g_fcSpeed_P = 8 ;	//8
float code g_fcSpeed_I = 0.05 ;	//0.05
/******蓝牙控制参数******/
float xdata g_fBluetoothSpeed;
float xdata g_fBluetoothDirection;

/***************************************************************
** 函数名称: CarUpstandInit
** 功能描述: 外部中断初始化函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void EXTINTInit(void)//外部中断初始化函数
{

	//EX0 = 1;    //开外部中断INT0
    EX1 = 1;    //开外部中断INT1
    //IT0 = 1;    //下降沿触发
    IT1 = 1;    //下降沿触发

}

/***************************************************************
** 函数名称: CarUpstandInit
** 功能描述: 全局变量初始化函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void CarUpstandInit()
{
	g_iAccelInputVoltage_Y_Axis = g_iGyroInputVoltage_X_Axis = 0;
	g_liLeftMotorPulse = g_liRightMotorPulse = 0;

	g_fCarSpeed    = 0;
	g_fCarPosition = 0;
	g_fCarAngle    = 0;
	g_fGyroAngleSpeed = 0;
	g_fGravityAngle   = 0;
	g_fGyroscopeAngleIntegral = 0;
	g_fAngleControlOut = g_fSpeedControlOut = 0;
	g_fLeftMotorOut    = g_fRightMotorOut   = 0;
	g_fBluetoothSpeed  = g_fBluetoothDirection = 0;

    g_ucLEDCount = 0;
	g_ucSpeedCountCarry = 0;

}
/***************************************************************
** 函数名称: DataSynthesis
** 功能描述: MPU6050数据合成函数
** 输　入:   REG_Address : 地址
** 输　出:   合成数据
** 全局变量: 
***************************************************************/
int DataSynthesis(unsigned char REG_Address)	
{
	int uiHighByte; /*高八位*/
	char ucLowByte; /*低八位*/

	uiHighByte = Single_ReadI2C(REG_Address)  ;
	ucLowByte  = Single_ReadI2C(REG_Address+1);

	return ((uiHighByte << 8) + ucLowByte);   /*返回合成数据*/
}
/***************************************************************
** 函数名称: SampleInputVoltage
** 功能描述: 采样函数             
** 输　入:   
** 输　出:   
***************************************************************/
void SampleInputVoltage(void)
{	
#if 0 
    /*
	N次均值滤波，此处N取20。
	属于低通滤波的一种，会降低频响，N值要适当，不宜过大过小。
	*/	
	unsigned char ucValue;   
	for(ucValue = 0 ; ucValue < 20 ; ucValue ++)
	{
		g_iAccelInputVoltage_Y_Axis  = DataSynthesis(ACCEL_YOUT_H);//加速度Y轴
		g_iGyroInputVoltage_X_Axis = DataSynthesis(GYRO_XOUT_H);//陀螺仪X轴
	
		g_liAccAdd += g_iAccelInputVoltage_Y_Axis;
		g_liGyroAdd += g_iGyroInputVoltage_X_Axis;			
						
	} 	

	g_iAccelInputVoltage_Y_Axis = g_liAccAdd  / 20 ;
    g_iGyroInputVoltage_X_Axis  = g_liGyroAdd / 20 ;

	g_liAccAdd  = 0;	  /*滤波完全局变量要清零，下次调用才不会出错*/
    g_liGyroAdd = 0;

#else 	/*不作任何滤波处理*/

        g_iAccelInputVoltage_Y_Axis  = DataSynthesis(ACCEL_YOUT_H);//加速度Y轴
		g_iGyroInputVoltage_X_Axis   = DataSynthesis(GYRO_XOUT_H) ;//陀螺仪X轴

#endif	
}

/***************************************************************
** 函数名称: SetMotorVoltageAndDirection
** 功能描述: 电机转速及方向控制函数             
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void SetMotorVoltageAndDirection(int iLeftVoltage,int iRightVoltage)
{
	unsigned char ucLeftMotorValue;
	unsigned char ucRighttMotorValue;
	
    if(iLeftVoltage<0)
    {	
      M1 = 0;				      //右电机前进	角度为负  速度为正
      M2 = 1;
      ucLeftMotorValue = (-iLeftVoltage);
    }
    else 
    {	
      M1 = 1;				      //右电机后退  角度为正  速度为负
      M2 = 0; 
      ucLeftMotorValue = iLeftVoltage;
    }

    if(iRightVoltage<0)
    {	
      M3 = 1;				      //左电机前进  角度为负  速度为正
      M4 = 0;
      ucRighttMotorValue = (-iRightVoltage);
    }
    else
    {	
      M3 = 0;				      //左电机后退  角度为正  速度为负
      M4 = 1; 
      ucRighttMotorValue = iRightVoltage;
    }

    if(ucLeftMotorValue   == 0)	CCAP0H=255;
    if(ucRighttMotorValue == 0)	CCAP1H=255;

	ucLeftMotorValue   = (255 - ucLeftMotorValue)  ;	   
	ucRighttMotorValue = (255 - ucRighttMotorValue);
	
  	CCAP0H = CCAP0L = ucLeftMotorValue  ;  //设定PWM0占空比(CCAP0H=255，速度最小)
   	CCAP1H = CCAP1L = ucRighttMotorValue;  //设定PWM1占空比(CCAP0H=255，速度最小)

#if IF_CAR_FALL		 /*判断车辆是否跌倒，调试用*/

	if(g_fCarAngle > 70 || g_fCarAngle < (-70))
	{
		CCAP0H = 255 ;	//占空比为零
   		CCAP1H = 255 ;    
	}

#endif
}

/***************************************************************
** 函数名称: MotorOutput
** 功能描述: 电机输出函数
             将直立控制、速度控制、方向控制的输出量进行叠加,并加
			 入死区常量，对输出饱和作出处理。
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void MotorOutput(void)
{
#if EN_ANGLE_LOOP
	g_fLeftMotorOut = g_fAngleControlOut ;
	g_fRightMotorOut = g_fAngleControlOut ;
#endif
#if EN_SPEED_LOOP
	g_fLeftMotorOut = g_fAngleControlOut - g_fSpeedControlOut;
	g_fRightMotorOut = g_fAngleControlOut - g_fSpeedControlOut;
#endif
#if EN_DIRECTION_LOOP
	g_fLeftMotorOut  = g_fAngleControlOut - g_fSpeedControlOut \
	+ g_fBluetoothDirection ;
	//+ g_fBluetoothSpeed + g_fBluetoothDirection ;
	
	g_fRightMotorOut = g_fAngleControlOut - g_fSpeedControlOut \
	- g_fBluetoothDirection ;
	//+ g_fBluetoothSpeed - g_fBluetoothDirection ;
	
#endif
	
	/*增加死区常数*/
	if(g_fLeftMotorOut>0)       g_fLeftMotorOut  += MOTOR_OUT_DEAD_VAL;
	else if(g_fLeftMotorOut<0)  g_fLeftMotorOut  -= MOTOR_OUT_DEAD_VAL;
	if(g_fRightMotorOut>0)      g_fRightMotorOut += MOTOR_OUT_DEAD_VAL;
	else if(g_fRightMotorOut<0) g_fRightMotorOut -= MOTOR_OUT_DEAD_VAL;

	/*输出饱和处理，防止超出PWM范围*/	
	if(g_fLeftMotorOut  > MOTOR_OUT_MAX)	g_fLeftMotorOut  = MOTOR_OUT_MAX;
	if(g_fLeftMotorOut  < MOTOR_OUT_MIN)	g_fLeftMotorOut  = MOTOR_OUT_MIN;
	if(g_fRightMotorOut > MOTOR_OUT_MAX)	g_fRightMotorOut = MOTOR_OUT_MAX;
	if(g_fRightMotorOut < MOTOR_OUT_MIN)	g_fRightMotorOut = MOTOR_OUT_MIN;

    SetMotorVoltageAndDirection((int)g_fLeftMotorOut,(int)g_fRightMotorOut);
}

/***************************************************************
** 函数名称: SpeedControl
** 功能描述: 速度环控制函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void SpeedControl(void)
{
    
    g_liRightMotorPulse = g_ucSpeedCountCarry * 65536 + TH0 * 256 + TL0;
	g_ucSpeedCountCarry = 0;	
	TH0 = TL0 = 0 ;
	/*通过PWM输出判断电机转向*/
	if(!MOTOR_LEFT_SPEED_POSITIVE)  g_liLeftMotorPulse  = -g_liLeftMotorPulse ; 
	if(!MOTOR_RIGHT_SPEED_POSITIVE) g_liRightMotorPulse = -g_liRightMotorPulse;
	
	g_fCarSpeed = (float)(g_liLeftMotorPulse  + g_liRightMotorPulse ) * 0.5;
    g_liLeftMotorPulse = g_liRightMotorPulse = 0;	  //全局变量 注意及时清零

	/*低通滤波*/
    g_fCarSpeed = g_fCarSpeedOld * 0.9 + g_fCarSpeed * 0.1 ;
	g_fCarSpeedOld = g_fCarSpeed;	
    	

	g_fCarSpeed *= CAR_SPEED_CONSTANT;	 //单位：转/秒
	g_fCarPosition += g_fCarSpeed; 		 //路程  即速度积分
	g_fCarPosition += g_fBluetoothSpeed;
	/*积分上限设限*/
	if((int)g_fCarPosition > SPEED_CONTROL_OUT_MAX)    g_fCarPosition = SPEED_CONTROL_OUT_MAX;
	if((int)g_fCarPosition < SPEED_CONTROL_OUT_MIN)    g_fCarPosition = SPEED_CONTROL_OUT_MIN;

	g_fSpeedControlOut = (CAR_SPEED_SET - g_fCarSpeed) * g_fcSpeed_P + \
	(CAR_SPEED_SET - g_fCarPosition) * g_fcSpeed_I; 

}

/***************************************************************
** 函数名称: AngleControl
** 功能描述: 角度环控制函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void AngleControl(void)	 
{
	float fDeltaValue;
	g_fGravityAngle = (g_iAccelInputVoltage_Y_Axis - GRAVITY_OFFSET) * GRAVITY_ANGLE_RATIO ;
	g_fGyroAngleSpeed = (g_iGyroInputVoltage_X_Axis - GYRO_OFFSET) * GYROSCOPE_ANGLE_RATIO  ;	
							 
	g_fCarAngle = g_fGyroscopeAngleIntegral ;
	
	fDeltaValue = (g_fGravityAngle - g_fCarAngle) / GRAVITY_ADJUST_TIME_CONSTANT;
	g_fGyroscopeAngleIntegral += (g_fGyroAngleSpeed + fDeltaValue) / GYROSCOPE_ANGLE_SIGMA_FREQUENCY;

	g_fAngleControlOut = (CAR_ANGLE_SET - g_fCarAngle)* g_fcAngle_P + \
	(CAR_ANGULARSPEED_SET - g_fGyroAngleSpeed )* g_fcAngle_D ;

}

/***************************************************************
** 函数名称: BluetoothControl
** 功能描述: 蓝牙控制函数
             手机发送内容
			 上：00000010    下：00000001
             左：00000011    右：00000100
             停止：00000000
             功能键（可自编下位机程序扩展）：
             A:00000101      B:00000110
             C:00000111      D:00001000
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void BluetoothControl(void)	 
{
	unsigned char ucBluetoothValue;

	ucBluetoothValue = UART1ReceiveByte();		
	switch (ucBluetoothValue)
	{
	  case 0x02 : g_fBluetoothSpeed =   7 ;  break;	   //前进
	  case 0x01 : g_fBluetoothSpeed = (-30);  break;	   //后退
	  case 0x03 : g_fBluetoothDirection =   7 ;  break;//左转
	  case 0x04 : g_fBluetoothDirection = (-30);  break;//右转
	  case 0x05 : g_fBluetoothSpeed =   7 ; break ;
	  case 0x06 : g_fBluetoothSpeed = (-30); break ;
	  case 0x07 : g_fBluetoothDirection =   7 ;  break;
	  case 0x08 : g_fBluetoothDirection = (-30);  break;
	  default : g_fBluetoothSpeed = 0; g_fBluetoothDirection = 0; LED2=~LED2;break;
	}
	
}