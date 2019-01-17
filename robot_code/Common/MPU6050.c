
#include "MPU6050.H"

/***************************************************************
** 函数名称: MPU6050Init
** 功能描述: MPU6050初始化函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void MPU6050Init(void)
{
	Single_WriteI2C(PWR_MGMT_1, 0x00)  ; //解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07)  ;
	Single_WriteI2C(CONFIG, 0x06)      ;
	Single_WriteI2C(GYRO_CONFIG, 0x18) ;
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}








