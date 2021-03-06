
#include <uart.h>   

/***************************************************************
** 函数名称: Uart1Init
** 功能描述: UART1初始化函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void Uart1Init(void)		//9600bps@20.000MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xBF;		    //设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
}

/***************************************************************
** 函数名称: Uart2Init
** 功能描述: UART1初始化函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void Uart2Init(void)		//9600bps@20.000MHz
{
	AUXR1 |= 0x10;       //UART2从P1口切换到P4口
	AUXR &= 0xF7;		//波特率不倍速
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xBF;		    //设定独立波特率发生器重装值
	AUXR |= 0x10;		//启动独立波特率发生器
}

/***************************************************************
** 函数名称: UART1ReceiveByte
** 功能描述: UART1接收函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
unsigned char UART1ReceiveByte(void)
{
    unsigned char xdata ucRxd1; 
    /*if(RI == 1)					     
    {
      RI = 0;
      ucRxd1 = SBUF;
      return(ucRxd1);		 
    }
    //return 0;	  */
	while(RI==0);
	RI = 0;
	ucRxd1 = SBUF;
	return(ucRxd1);
}

/***************************************************************
** 函数名称: UART2ReceiveByte
** 功能描述: UART2接收函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
unsigned char UART2ReceiveByte(void)
{
    unsigned char xdata ucRxd2; /*
	if ((S2CON & 0x01) == 1)  
    {  
      S2CON &= 0xFE;  
      ucRxd2 = S2BUF;
	  return(ucRxd2);  
    }     		 	   */
	while((S2CON & 0x01) == 0);
	S2CON &= 0xFE;
	ucRxd2 = S2BUF;
	return(ucRxd2);
}

/***************************************************************
** 函数名称: UART1SendByte
** 功能描述: UART1发送函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void UART1SendByte(unsigned char TxD1)  
{   
    SBUF=TxD1;  
    while(TI == 0);//等待发送完成 
    //while(!TI);//等待发送完成 
    TI=0;   
}  

/***************************************************************
** 函数名称: UART2SendByte
** 功能描述: UART2发送函数
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
void UART2SendByte(unsigned char TxD2)  
{   
    S2BUF=TxD2;  
    while ((S2CON & 0x02) == 0);  
    S2CON &= 0xFD;    
}   