
#ifndef _UPSTANDINGCAR_H_
#define _UPSTANDINGCAR_H_

/******Target CPU相关宏定义******/
#define CLK_DIV_1() CLK_DIV &= 0xF8 /*CPU工作频率等于晶振频率*/
/*
	  CLKS2 CLKS1 CLKS0         分频后CPU实际工作时钟
	    0     0     0       外部晶体时钟或内部R/C振荡时钟
	    0     0     1       (外部晶体时钟或内部R/C振荡时钟)/2
	    0     1     0       (外部晶体时钟或内部R/C振荡时钟)/4
	    0     1     1       (外部晶体时钟或内部R/C振荡时钟)/8
	    1     0     0       (外部晶体时钟或内部R/C振荡时钟)/16
        1     0     1       (外部晶体时钟或内部R/C振荡时钟)/32
        1     1     0       (外部晶体时钟或内部R/C振荡时钟)/64
        1     1     1       (外部晶体时钟或内部R/C振荡时钟)/128	
*/	
#define DisableInterrupts	EA=0
#define EnableInterrupts 	EA=1
sbit M1	= P1^0;		  //电机驱动(M1=1，M2=0，右电机前进)
sbit M2	= P1^1;		  //电机驱动(M1=0，M2=1，右电机后退)
sbit M3	= P1^5;		  //电机驱动(M3=1，M4=0，左电机后退)
sbit M4	= P1^6;		  //电机驱动(M3=0，M4=1，左电机前进)
sbit LeftMotorDirection = P3^4;
sbit RightMotorDirection= P3^5;              //A脉冲   B脉冲接外部中断引脚 
/******角度控制相关宏定义******/
#define     CAR_ANGLE_SET	      0
#define     CAR_ANGULARSPEED_SET  0
#define     GRAVITY_OFFSET (-1000)       //加速度零点偏移值 
#define     GYRO_OFFSET      0          //陀螺仪零点偏移值
#define     GRAVITY_X_MIN       (-16384)
#define     GRAVITY_X_MAX         16384     
#define     CAR_ANGLE_RANGE	  	 90
#define     GRAVITY_ANGLE_RATIO	 ((float)CAR_ANGLE_RANGE / ((float)GRAVITY_X_MAX - (float)GRAVITY_X_MIN) * 2.0)
#define     GYROSCOPE_ANGLE_RATIO     0.09	
#define 	GYROSCOPE_ANGLE_SIGMA_FREQUENCY 100	 //1/0.01=100
#define 	GRAVITY_ADJUST_TIME_CONSTANT 0.5 //3
/******速度控制相关宏定义******/
#define CAR_POSITION_SET      0
#define CAR_SPEED_SET         0
#define MOTOR_LEFT_SPEED_POSITIVE  ((int)g_fLeftMotorOut >0)
#define MOTOR_RIGHT_SPEED_POSITIVE ((int)g_fRightMotorOut>0)
#define OPTICAL_ENCODE_CONSTANT  333	//光电码盘刻度槽
#define SPEED_CONTROL_PERIOD	 10	    //速度环控制周期
#define CAR_SPEED_CONSTANT		(1000/SPEED_CONTROL_PERIOD/(float)OPTICAL_ENCODE_CONSTANT)
#define SPEED_CONTROL_OUT_MAX	(MOTOR_OUT_MAX*10) 
#define SPEED_CONTROL_OUT_MIN	(MOTOR_OUT_MIN*10) 
/******电机控制相关宏定义******/
#define MOTOR_OUT_DEAD_VAL       0 	   //死区值8
#define MOTOR_OUT_MAX           255	   //占空比正最大值
#define MOTOR_OUT_MIN         (-255)   //占空比负最大值
/******调试相关宏定义******/
#define IF_CAR_FALL	1		//判断车模是否跌倒		   
#define EN_ANGLE_LOOP 0     
#define EN_SPEED_LOOP 0
#define EN_DIRECTION_LOOP 1




extern unsigned char g_ucLEDCount;
extern unsigned char g_ucSpeedCountCarry;
extern long int g_liLeftMotorPulse;
extern long int g_liRightMotorPulse;
extern float g_fCarAngle;
extern float g_fGravityAngle;
extern float g_fGyroAngleSpeed ;
extern int g_iAccelInputVoltage_Y_Axis;


void EXTINTInit(void);
int  DataSynthesis(unsigned char REG_Address);
void SampleInputVoltage(void);
void CarUpstandInit(void);
void AngleCalculate(void);
void SetMotorVoltageAndDirection(int iLeftVoltage,int iRightVoltage);
void MotorOutput(void);
void SpeedControl(void);
void AngleControl(void);
void BluetoothControl(void);	

#endif