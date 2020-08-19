#include "Delay.h"

void TIM2_init()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef Tim2;
	Tim2.TIM_Prescaler = 72-1;
	Tim2.TIM_Period = 1000;
	TIM_TimeBaseInit(TIM2, &Tim2);
	TIM_Cmd(TIM2, ENABLE);
}  
		
void delay_us(uint32_t us)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < us) {}
}
		
void delay_ms(uint32_t ms)
{
	for(uint32_t i = 0; i < ms; ++i)
	delay_us(1000);
}
