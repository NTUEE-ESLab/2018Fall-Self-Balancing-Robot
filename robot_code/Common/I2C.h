
/*****************************************************

  普通IO模拟I2C通信
  芯片：STC12C5A60S2
  晶振：20MHz

*****************************************************/

#ifndef  __I2C_H__
#define  __I2C_H__


#include <stc12c5a60s2.H>	
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include <intrins.H>



//********端口定义**********************************
sbit    SCL = P0^1;			//I2C时钟引脚定义
sbit    SDA = P0^0;			//I2C数据引脚定义

#define	SlaveAddress 0xD0   //IIC写入时的地址字节数据，+1为读取


//********函数初始声明******************************
void  Delay5us();
void  I2C_Start();
void  I2C_Stop();
void  I2C_SendACK(bit ack);
bit   I2C_RecvACK();
void  I2C_SendByte(unsigned char dat);
unsigned char I2C_RecvByte();
unsigned char Single_ReadI2C(unsigned char REG_Address);						//读取I2C数据
void  Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data);	    //向I2C写入数据






#endif