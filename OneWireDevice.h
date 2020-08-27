#ifndef _ONEWIREDEVICE_H_
#define _ONEWIREDEVICE_H_
#include "lcd1602.h"

//#include <string>
//#define _1w_to_gnd GPIOA->ODR&=~0x1 //подтяжка к земле
//#define _1w_to_up GPIOA->ODR|=0x1 // отпускаем шину
//#define read_bit_on_1w GPIOA->IDR&0x1 // считка состояние пина 1w

#define SKIP_ROM 0xCC
#define CONVERT_T 0x44
#define READ 0xBE



class OneWireDevice
{
	protected:
		//Объявление полей класса
	GPIO_TypeDef* m_1w_port; //указатель на структуру(здесь будет имя порта)
	unsigned short m_1w_pin; // пин, куда подключен датчик
	unsigned m_1w_clock;	// нужно для включения тактирования
	unsigned char m_buf[9];
	
		
	//Protected методы, доступ к которым извне будет недоступен
		void _1wToUp()//отпускаем шину
		{
				m_1w_port->ODR |= m_1w_pin;
		}
		
		void _1wToGnd()//подтяжка к земле
		{
				m_1w_port->ODR &= ~m_1w_pin;
		}
		
		bool readBitFromOneWire()//вызращаем результат считывания уровня напряжения на пине 
		{
			return m_1w_port->IDR & m_1w_pin;
		}
		
		bool resetOneWire()
		{
			bool reset = false;
			_1wToUp(); // отпускаем шину
			_1wToGnd(); // притягиваем к земле
			delay_us(485);
			_1wToUp(); // отпускаем шину
			delay_us(70); // задержка 70 мкс
			reset = readBitFromOneWire();
			delay_us(440); // задержка 440 мкс
			return reset;
		}
		
		void writeBit(unsigned char bit)
		{
			_1wToGnd(); //притягиваем шину к земле
			if(bit)
			{
				delay_us(3);
				_1wToUp(); // отпускаем
			}
			delay_us(65);
			_1wToUp(); // отпускаем
			delay_us(1);
		}

//---------------------------------------------------------------------------------------------------------------------------------

		void writeByte(unsigned int byte)
		{
			for(char i = 0; i < 8; i++)
			{
				if((byte & (1<<i)) == 1<<i)
					writeBit(1);
				else 
					writeBit(0);
			}
		}


//---------------------------------------------------------------------------------------------------------------------------------

		char readBit(void)
		{
			unsigned char bit = 0;
			_1wToUp(); //отпускаем шину
			delay_us(1);
			_1wToGnd(); //притягиваем шину к земле
			delay_us(3);
			_1wToUp(); //отпускаем шину
			delay_us(12);
			bit = readBitFromOneWire();
			delay_us(50); // задержка 50 мкс
			return bit;
		}
//---------------------------------------------------------------------------------------------------------------------------------

		unsigned char readByte(void)
		{
			unsigned char byte = 0;
			for(unsigned char i = 0; i < 8; i++)
			{
				byte |= readBit()<<i;
			}
			return byte;
		}
		
		//virtual void getMemory() = 0;
		
		
		OneWireDevice(GPIO_TypeDef* _1w_port, unsigned short _1w_pin, unsigned _1w_clock):m_1w_clock(_1w_clock),m_1w_pin(_1w_pin), m_1w_port(_1w_port)
		{
			RCC_APB2PeriphClockCmd(m_1w_clock, ENABLE);
			GPIO_InitTypeDef PA;
			PA.GPIO_Pin = m_1w_pin;
			PA.GPIO_Mode = GPIO_Mode_Out_OD;
			PA.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(m_1w_port, &PA);
		}
		
		//*********************************************************
		//Эти методы не будут работать без инициализации таймера!!!
		//Поэтому перед созданием объекта класса, нужно вызвать функцию инициализации TIM2_Init()
		//*********************************************************
		//Пока функционал класса ограничен только ситыванием температуры без поиска устройств на линии
		//*********************************************************

		
		
		
		//void searchROM();
		
		//void readROM();
		
		//virtual unsigned char calculateCrc() = 0;
		//virtual unsigned char getCRCFromDevice() = 0;

};

class Ds18b20 : public OneWireDevice
{
	private:
		enum bytesOfDevice{LOW_BYTE_OF_TEMP = 0, HIGH_BYTE_OF_TEMP, T_H, T_L, REG_OF_CONF, RES1, RES2, RES3, CRC_CODE}; // байты в порядке их передачи from slave into master
		// массив байт считанных из датчика
		float m_temperature;
		char m_str_temperature[7];
		unsigned char rom_buf[8];
		
		void getMemory()
			{
				resetOneWire();// сигнал сброса
				writeByte(SKIP_ROM); //пропустить считку ROM датчика
				writeByte(CONVERT_T); // сигнал "конвертируй температуру"
				delay_ms(1000);// задержка необходима для конвертации температуры (зависит от разрядности)
				
				resetOneWire();// опять сброс
				writeByte(SKIP_ROM);//опять пропускаем считку ROM
				writeByte(READ);// считываем память. датчик передаст 9 байт, но нам нужны только первые 2, отвечающих за температуру.

				for(char i = 0; i < 9; i++)
				{
					m_buf[i] = readByte();
				}
			}
		
	public:
		Ds18b20(GPIO_TypeDef* _1w_port, unsigned short _1w_pin, unsigned _1w_clock):OneWireDevice(_1w_port, _1w_pin, _1w_clock)
		{
			m_temperature = 0;
		}
		
		
		
			
			//-------------------------------------------------------------------------------------------------------
			float getFTemperature()
			{
				getMemory(); // получаем 9 байт в массив
				unsigned short temperature = 0;
				temperature = m_buf[HIGH_BYTE_OF_TEMP] << 8 | m_buf[LOW_BYTE_OF_TEMP]; // записываем в переменную первые два байта с температурой
				m_temperature = (temperature >> 4) + (static_cast<float>(temperature & 0xF))/16;// первый четыре бита младшего байта температуры это дробная часть, поэтому делим на 16
				
				return m_temperature;
			}
			//-------------------------------------------------------------------------------------------------------
			
			unsigned char getCRCFromDevice()
			{
				return m_buf[CRC_CODE];
			}
			
			
			//--------------------------------------------------конвертация float в Си строку
			char* temperatureToString()
			{
				if(m_temperature >= 10.0 && m_temperature < 100.0)
				{
					sprintf(m_str_temperature, "%f", m_temperature);//буфер для преобразования типа float в си-строку
					m_str_temperature[5] = '\0';
				}
				if(m_temperature < 10.0 && m_temperature > -10.0)
				{
					sprintf(m_str_temperature, "%f", m_temperature);//буфер для преобразования типа float в си-строку
					m_str_temperature[4] = '\0';
				}
				return m_str_temperature;
			}
			
			
			//Расчет CRC
			unsigned char calculateCrc()
			{
				unsigned char *pcBlock = m_buf; // указатель на первый элемент массива данных датчика
				int len = 8; // длина массива до байта crc
				unsigned char crc = 0x0;
				while(len--)
				{
					unsigned char inbyte = *pcBlock++;
					for(char i = 0; i < 8; i++)
					{
						unsigned char mix = (crc^inbyte) & 0x01;
						crc >>= 1;
						if(mix) {crc ^= 0x8C;}
						inbyte >>= 1;
					}
				}
				return crc;
			}
};
#endif