#ifndef   _IAP_H_
#define   _IAP_H_
//慧净电子：WWW.HJMCU.COM   WWW.HLMCU.COM
#include "stc12c5a60s2.h"

sbit  IN_OFF=P3^0;//串口接收端

void STC_ISP();

#endif