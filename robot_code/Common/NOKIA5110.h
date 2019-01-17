

#ifndef __NOKIA5110_H__
#define __NIKIA5110_H__

#include "STC12C5A60S2.H"

sbit res  = P2^7;  //复位,0复位
sbit sce  = P2^6;  //片选
sbit dc   = P2^5;  //1写数据，0写指令
sbit sdin = P2^4;  //数据
sbit sclk = P2^3;  //时钟

void delay_1ms(void);
void LCD_write_byte(unsigned char dt, unsigned char command);
void LCD_init(void);
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_clear(void) ;
void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c);
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c); //row:列 page:页 dd:字符

#endif