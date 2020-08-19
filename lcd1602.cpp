#include "lcd1602.h"

void LCD_pinsInit()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef PA; // Пины D4-D7
  PA.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
  PA.GPIO_Mode = GPIO_Mode_Out_PP;
  PA.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &PA);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
  
  GPIO_InitTypeDef PB; // пины RS и E
  PB.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  PB.GPIO_Mode = GPIO_Mode_Out_PP;
  PB.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &PB);
}

void sendhalfbyte(unsigned char byte)
{
  GPIOA->ODR &= ~0xF0;
  GPIOA->ODR |= byte;
  e1;
  delay_us(5);
  e0;
  delay_us(50);
}

void sendbyte(unsigned char byte, unsigned char mode) // mode = 0: rs0 - команда, mode = 1: данные 0xf1
{
  if(mode == 1) { rs1; }
	else { rs0; }
  
  unsigned char ml_byte = 0, st_byte = 0;
  ml_byte = byte << 4;
  st_byte = byte & 0xF0;
  sendhalfbyte(st_byte);
  sendhalfbyte(ml_byte);
}

void LCD_clear()
{
  sendbyte(0x1, 0);
  delay_ms(2);
}

void addDegreeSymbol()
{
	sendbyte(0x48, 0);// переключение адреса на CGRAM и запись символа по адресу 0x1
	delay_us(40);
	
	sendbyte(0x7, 1);
	delay_us(40);
	sendbyte(0x5, 1);
	delay_us(40);
	sendbyte(0x7, 1);
	delay_us(40);
	sendbyte(0x0, 1);
	delay_us(40);
	sendbyte(0x0, 1);
	delay_us(40);
	sendbyte(0x0, 1);
	delay_us(40);
	sendbyte(0x0, 1);
	delay_us(40);
	sendbyte(0x0, 1);
	delay_us(40);
	
	sendbyte(0x80, 0);// переключаемся обратно на DDRAM
	delay_us(100);	
}

void LCD_init() // 4-bit interface
{
  delay_ms(20);
  sendbyte(0x2, 0); // 4-bit interface
  delay_ms(5);
	sendbyte(0x20, 0);
	delay_us(100);
  sendbyte(0x20, 0);// выбор 4-bit интерфейса
  delay_us(100);
	
  sendbyte(0x2C, 0);// выбор числа линии и размер символа (1 line, 5x8)
  delay_us(100);

  sendbyte(0xC, 0); // display on, мерцание и курсор off
  delay_us(100);
  
  LCD_clear();
  
  
  sendbyte(0x6, 0); // позиция курсора, инкремент адреса, сдвиг вправо
  delay_us(100);
	
  addDegreeSymbol(); //запись в начало CGRAM символа градуса
}

void LCD_sendchar(uint16_t c)
{
  sendbyte(c,1);
}

void floatToLCD(float &temp)
{
	LCD_clear();
	char str_temp[7];
	if(temp >= 10.0 && temp < 100.0)
	{
		sprintf(str_temp, "%f", temp);// запись буфер типа float и извелечение значения в массив char-ов
		str_temp[5] = '\0';
	}
	if(temp < 10.0 && temp > -10.0)
	{
		sprintf(str_temp, "%f", temp);
		str_temp[4] = '\0';
	}
	
	for(char ch = 0; str_temp[ch] != '\0'; ch++)
  {
    LCD_sendchar(str_temp[ch]);// массив с температурой на LCD
  }
	
	LCD_sendchar(0x1); // символ градуса
	LCD_sendchar('C');
}

