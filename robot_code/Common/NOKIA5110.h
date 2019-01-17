

#ifndef __NOKIA5110_H__
#define __NIKIA5110_H__

#include "STC12C5A60S2.H"

sbit res  = P2^7;  //��λ,0��λ
sbit sce  = P2^6;  //Ƭѡ
sbit dc   = P2^5;  //1д���ݣ�0дָ��
sbit sdin = P2^4;  //����
sbit sclk = P2^3;  //ʱ��

void delay_1ms(void);
void LCD_write_byte(unsigned char dt, unsigned char command);
void LCD_init(void);
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_clear(void) ;
void LCD_write_shu(unsigned char row, unsigned char page,unsigned char c);
void LCD_write_hanzi(unsigned char row, unsigned char page,unsigned char c); //row:�� page:ҳ dd:�ַ�

#endif