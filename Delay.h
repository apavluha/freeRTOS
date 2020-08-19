#ifndef _DELAY_H_
#define _DELAY_H_
#include "stm32f10x.h"                  

extern void TIM2_init();

extern void delay_us(uint32_t us);

extern void delay_ms(uint32_t ms);

#endif
