#ifndef __LED_H__
#define __LED_H__

#include "stc12c5a60s2.h"

sbit LED0 = P5^0;
sbit LED1 = P5^1;
sbit LED2 = P5^2;
sbit LED3 = P5^3;

void LEDInit(void);

#endif