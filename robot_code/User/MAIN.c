//51单片机平衡小车源码
//只供参考学习，不得商用，不提供源码分析
//请用UV4编译，出现WARNING 是正常现象
//小车平衡度不好时，有条件的同学位可以用显波显测试PID参数，详细的方法请查看图文教程
#include "includes.h"
/***************************************************************
** 函数名称:  main
** 功能描述: 主函数            
***************************************************************/
void main()
{
	DisableInterrupts;//禁止总中断
	/*硬件底层初始化*/
	CLK_DIV_1();
	Timer0Init();	
	Timer1Init();
	PWMInit();        //调用PWM函数
	Uart1Init();     //调用串口1函数
	EXTINTInit();	
	MPU6050Init();	//调用平衡模块函数
	CarUpstandInit();
	Delaynms(500);	  //延时0.5s
	EnableInterrupts; //允许总中断
	
	while(1)
	{
    STC_ISP();	          //ISP 下载不用冷启动 本函数不用
		BluetoothControl();	  //蓝牙控制函数

/*调试用 确定轮子方向*/
#if 0	
		M1=1;
		M2=0;
		M3=0;
		M4=1;
		CCAP1H = CCAP1L = 0;
#endif					
	}
}
