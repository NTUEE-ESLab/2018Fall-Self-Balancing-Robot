

#include "includes.h"

unsigned char g_ucLEDCount;
/******������Ʋ���******/
float g_fSpeedControlOut;
float g_fAngleControlOut;
float g_fLeftMotorOut;
float g_fRightMotorOut;
/******�Ƕȿ��Ʋ���******/
int   g_iAccelInputVoltage_Y_Axis ;	//���ٶ�Y������
int   g_iGyroInputVoltage_X_Axis  ;	//������X������
//long int xdata g_liAccAdd;
//long int xdata g_liGyroAdd;
float g_fCarAngle;         			//��ģ���
float g_fGyroAngleSpeed;			//���ٶ�      			
float g_fGyroscopeAngleIntegral;	//���ٶȻ���ֵ
float g_fGravityAngle;				//���ٶȳ�������õ������
/******�ٶȿ��Ʋ���******/
long int   g_liLeftMotorPulse;
long int   g_liRightMotorPulse;
float g_fCarSpeed;
float g_fCarSpeedOld;
float g_fCarPosition;
unsigned char g_ucSpeedCountCarry;
/*-----�ǶȻ����ٶȻ�PID���Ʋ���-----*/
float code g_fcAngle_P = 12;	//12		������ֱ�� ��ֵ��ڶ���
float code g_fcAngle_D = 0.9;	//0.9 	 ����ʱ������ ����΢���ͺ�
float code g_fcSpeed_P = 8 ;	//8
float code g_fcSpeed_I = 0.05 ;	//0.05
/******�������Ʋ���******/
float xdata g_fBluetoothSpeed;
float xdata g_fBluetoothDirection;

/***************************************************************
** ��������: CarUpstandInit
** ��������: �ⲿ�жϳ�ʼ������
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void EXTINTInit(void)//�ⲿ�жϳ�ʼ������
{

	//EX0 = 1;    //���ⲿ�ж�INT0
    EX1 = 1;    //���ⲿ�ж�INT1
    //IT0 = 1;    //�½��ش���
    IT1 = 1;    //�½��ش���

}

/***************************************************************
** ��������: CarUpstandInit
** ��������: ȫ�ֱ�����ʼ������
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
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
** ��������: DataSynthesis
** ��������: MPU6050���ݺϳɺ���
** �䡡��:   REG_Address : ��ַ
** �䡡��:   �ϳ�����
** ȫ�ֱ���: 
***************************************************************/
int DataSynthesis(unsigned char REG_Address)	
{
	int uiHighByte; /*�߰�λ*/
	char ucLowByte; /*�Ͱ�λ*/

	uiHighByte = Single_ReadI2C(REG_Address)  ;
	ucLowByte  = Single_ReadI2C(REG_Address+1);

	return ((uiHighByte << 8) + ucLowByte);   /*���غϳ�����*/
}
/***************************************************************
** ��������: SampleInputVoltage
** ��������: ��������             
** �䡡��:   
** �䡡��:   
***************************************************************/
void SampleInputVoltage(void)
{	
#if 0 
    /*
	N�ξ�ֵ�˲����˴�Nȡ20��
	���ڵ�ͨ�˲���һ�֣��ή��Ƶ�죬NֵҪ�ʵ������˹����С��
	*/	
	unsigned char ucValue;   
	for(ucValue = 0 ; ucValue < 20 ; ucValue ++)
	{
		g_iAccelInputVoltage_Y_Axis  = DataSynthesis(ACCEL_YOUT_H);//���ٶ�Y��
		g_iGyroInputVoltage_X_Axis = DataSynthesis(GYRO_XOUT_H);//������X��
	
		g_liAccAdd += g_iAccelInputVoltage_Y_Axis;
		g_liGyroAdd += g_iGyroInputVoltage_X_Axis;			
						
	} 	

	g_iAccelInputVoltage_Y_Axis = g_liAccAdd  / 20 ;
    g_iGyroInputVoltage_X_Axis  = g_liGyroAdd / 20 ;

	g_liAccAdd  = 0;	  /*�˲���ȫ�ֱ���Ҫ���㣬�´ε��òŲ������*/
    g_liGyroAdd = 0;

#else 	/*�����κ��˲�����*/

        g_iAccelInputVoltage_Y_Axis  = DataSynthesis(ACCEL_YOUT_H);//���ٶ�Y��
		g_iGyroInputVoltage_X_Axis   = DataSynthesis(GYRO_XOUT_H) ;//������X��

#endif	
}

/***************************************************************
** ��������: SetMotorVoltageAndDirection
** ��������: ���ת�ټ�������ƺ���             
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void SetMotorVoltageAndDirection(int iLeftVoltage,int iRightVoltage)
{
	unsigned char ucLeftMotorValue;
	unsigned char ucRighttMotorValue;
	
    if(iLeftVoltage<0)
    {	
      M1 = 0;				      //�ҵ��ǰ��	�Ƕ�Ϊ��  �ٶ�Ϊ��
      M2 = 1;
      ucLeftMotorValue = (-iLeftVoltage);
    }
    else 
    {	
      M1 = 1;				      //�ҵ������  �Ƕ�Ϊ��  �ٶ�Ϊ��
      M2 = 0; 
      ucLeftMotorValue = iLeftVoltage;
    }

    if(iRightVoltage<0)
    {	
      M3 = 1;				      //����ǰ��  �Ƕ�Ϊ��  �ٶ�Ϊ��
      M4 = 0;
      ucRighttMotorValue = (-iRightVoltage);
    }
    else
    {	
      M3 = 0;				      //��������  �Ƕ�Ϊ��  �ٶ�Ϊ��
      M4 = 1; 
      ucRighttMotorValue = iRightVoltage;
    }

    if(ucLeftMotorValue   == 0)	CCAP0H=255;
    if(ucRighttMotorValue == 0)	CCAP1H=255;

	ucLeftMotorValue   = (255 - ucLeftMotorValue)  ;	   
	ucRighttMotorValue = (255 - ucRighttMotorValue);
	
  	CCAP0H = CCAP0L = ucLeftMotorValue  ;  //�趨PWM0ռ�ձ�(CCAP0H=255���ٶ���С)
   	CCAP1H = CCAP1L = ucRighttMotorValue;  //�趨PWM1ռ�ձ�(CCAP0H=255���ٶ���С)

#if IF_CAR_FALL		 /*�жϳ����Ƿ������������*/

	if(g_fCarAngle > 70 || g_fCarAngle < (-70))
	{
		CCAP0H = 255 ;	//ռ�ձ�Ϊ��
   		CCAP1H = 255 ;    
	}

#endif
}

/***************************************************************
** ��������: MotorOutput
** ��������: ����������
             ��ֱ�����ơ��ٶȿ��ơ�������Ƶ���������е���,����
			 �����������������������������
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
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
	
	/*������������*/
	if(g_fLeftMotorOut>0)       g_fLeftMotorOut  += MOTOR_OUT_DEAD_VAL;
	else if(g_fLeftMotorOut<0)  g_fLeftMotorOut  -= MOTOR_OUT_DEAD_VAL;
	if(g_fRightMotorOut>0)      g_fRightMotorOut += MOTOR_OUT_DEAD_VAL;
	else if(g_fRightMotorOut<0) g_fRightMotorOut -= MOTOR_OUT_DEAD_VAL;

	/*������ʹ�����ֹ����PWM��Χ*/	
	if(g_fLeftMotorOut  > MOTOR_OUT_MAX)	g_fLeftMotorOut  = MOTOR_OUT_MAX;
	if(g_fLeftMotorOut  < MOTOR_OUT_MIN)	g_fLeftMotorOut  = MOTOR_OUT_MIN;
	if(g_fRightMotorOut > MOTOR_OUT_MAX)	g_fRightMotorOut = MOTOR_OUT_MAX;
	if(g_fRightMotorOut < MOTOR_OUT_MIN)	g_fRightMotorOut = MOTOR_OUT_MIN;

    SetMotorVoltageAndDirection((int)g_fLeftMotorOut,(int)g_fRightMotorOut);
}

/***************************************************************
** ��������: SpeedControl
** ��������: �ٶȻ����ƺ���
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void SpeedControl(void)
{
    
    g_liRightMotorPulse = g_ucSpeedCountCarry * 65536 + TH0 * 256 + TL0;
	g_ucSpeedCountCarry = 0;	
	TH0 = TL0 = 0 ;
	/*ͨ��PWM����жϵ��ת��*/
	if(!MOTOR_LEFT_SPEED_POSITIVE)  g_liLeftMotorPulse  = -g_liLeftMotorPulse ; 
	if(!MOTOR_RIGHT_SPEED_POSITIVE) g_liRightMotorPulse = -g_liRightMotorPulse;
	
	g_fCarSpeed = (float)(g_liLeftMotorPulse  + g_liRightMotorPulse ) * 0.5;
    g_liLeftMotorPulse = g_liRightMotorPulse = 0;	  //ȫ�ֱ��� ע�⼰ʱ����

	/*��ͨ�˲�*/
    g_fCarSpeed = g_fCarSpeedOld * 0.9 + g_fCarSpeed * 0.1 ;
	g_fCarSpeedOld = g_fCarSpeed;	
    	

	g_fCarSpeed *= CAR_SPEED_CONSTANT;	 //��λ��ת/��
	g_fCarPosition += g_fCarSpeed; 		 //·��  ���ٶȻ���
	g_fCarPosition += g_fBluetoothSpeed;
	/*������������*/
	if((int)g_fCarPosition > SPEED_CONTROL_OUT_MAX)    g_fCarPosition = SPEED_CONTROL_OUT_MAX;
	if((int)g_fCarPosition < SPEED_CONTROL_OUT_MIN)    g_fCarPosition = SPEED_CONTROL_OUT_MIN;

	g_fSpeedControlOut = (CAR_SPEED_SET - g_fCarSpeed) * g_fcSpeed_P + \
	(CAR_SPEED_SET - g_fCarPosition) * g_fcSpeed_I; 

}

/***************************************************************
** ��������: AngleControl
** ��������: �ǶȻ����ƺ���
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
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
** ��������: BluetoothControl
** ��������: �������ƺ���
             �ֻ���������
			 �ϣ�00000010    �£�00000001
             ��00000011    �ң�00000100
             ֹͣ��00000000
             ���ܼ������Ա���λ��������չ����
             A:00000101      B:00000110
             C:00000111      D:00001000
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void BluetoothControl(void)	 
{
	unsigned char ucBluetoothValue;

	ucBluetoothValue = UART1ReceiveByte();		
	switch (ucBluetoothValue)
	{
	  case 0x02 : g_fBluetoothSpeed =   7 ;  break;	   //ǰ��
	  case 0x01 : g_fBluetoothSpeed = (-30);  break;	   //����
	  case 0x03 : g_fBluetoothDirection =   7 ;  break;//��ת
	  case 0x04 : g_fBluetoothDirection = (-30);  break;//��ת
	  case 0x05 : g_fBluetoothSpeed =   7 ; break ;
	  case 0x06 : g_fBluetoothSpeed = (-30); break ;
	  case 0x07 : g_fBluetoothDirection =   7 ;  break;
	  case 0x08 : g_fBluetoothDirection = (-30);  break;
	  default : g_fBluetoothSpeed = 0; g_fBluetoothDirection = 0; LED2=~LED2;break;
	}
	
}