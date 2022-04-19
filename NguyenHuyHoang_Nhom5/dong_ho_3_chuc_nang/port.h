#ifndef _PORT_H_
#define _PORT_H_

//khai bao LCD
sbit LCD_RS	= P2^0;
sbit LCD_EN	= P2^2;
sbit LCD_D4	= P2^4;
sbit LCD_D5 = P2^5;
sbit LCD_D6 = P2^6;
sbit LCD_D7 = P2^7;

//khai bao I2C
sbit SOFT_I2C_SCL	= P1^0;
sbit SOFT_I2C_SDA	= P1^1;

//khai bao nut nhan
sbit BTN_NEXT = P3^0;
sbit BTN_UP = P3^1;
sbit BTN_DOWN = P3^2;

#endif