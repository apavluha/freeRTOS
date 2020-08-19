#ifndef _LCD_H_
#define _LCD_H_
#include "Delay.h" 
#include "stdio.h"

#define e1 GPIOB->ODR |= 0x1;
#define e0 GPIOB->ODR &= ~0x1;
#define rs1 GPIOB->ODR |= 0x2;
#define rs0 GPIOB->ODR &= ~0x2;

void LCD_pinsInit();

//---------------------------------------------------------------------------------------------------------------------------------

void sendhalfbyte(unsigned char byte);


//---------------------------------------------------------------------------------------------------------------------------------

void sendbyte(unsigned char byte, unsigned char mode); // mode = 0: rs0 - команда, mode = 1: данные 0xf1

//---------------------------------------------------------------------------------------------------------------------------------

void LCD_clear();

//---------------------------------------------------------------------------------------------------------------------------------

void addDegreeSymbol();

//---------------------------------------------------------------------------------------------------------------------------------

void LCD_init(); // 4-bit interface


//---------------------------------------------------------------------------------------------------------------------------------

void LCD_sendchar(uint16_t c);



//------------------------------------------------------------------------------------------------------------------------------

void floatToLCD(float &temp);

//------------------------------------------------------------------------------------------------------------------------------
//void convertTempToLCD(float temper, int column, int str)
//{
//		setpos(column, str);
//		int a = (int)temper;
//		int b = (int)((temper - a) * 100);
//    sendchar(a/10 + 0x30);
//    sendchar(a%10 + 0x30);
//    sendchar(0x2E);
//    sendchar(b/10 + 0x30);
//		sendchar(b%10 + 0x30);
//    sendchar(0x2A);
//		sendchar(0x43);
//}
//void LCD_setPosition(unsigned char x, unsigned char y)
//{
//  char address;
//  address = (0x40*y+x) | 0x80;
//  sendbyte(address, 0);
//  delay_us(100);
//}

//---------------------------------------------------------------------------------------------------------------------------------

//void sendchar(uint16_t c, unsigned char column_curs_position, unsigned char str_curs_position)
//{
//	setpos(column_curs_position, str_curs_position);
//  sendbyte(c,1);
//  delay_us(100);
//}

//------------------------------------------------------------------------------------------------------------------------------

//void str_lcd(char *str)
//{
//  char n;
//  for(n = 0; str[n] != '\0'; n++)
//  {
//    sendchar(str[n]);
//  }
//}
#endif	
