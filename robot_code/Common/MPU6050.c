
#include "MPU6050.H"

/***************************************************************
** ��������: MPU6050Init
** ��������: MPU6050��ʼ������
** �䡡��:   
** �䡡��:   
** ȫ�ֱ���: 
***************************************************************/
void MPU6050Init(void)
{
	Single_WriteI2C(PWR_MGMT_1, 0x00)  ; //�������״̬
	Single_WriteI2C(SMPLRT_DIV, 0x07)  ;
	Single_WriteI2C(CONFIG, 0x06)      ;
	Single_WriteI2C(GYRO_CONFIG, 0x18) ;
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}








