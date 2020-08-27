#include "OneWireDevice.h"                 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

			
void LedsInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);// инициализация и тактирование светодиодов
	GPIO_InitTypeDef PC;
	PC.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 ;
	PC.GPIO_Mode = GPIO_Mode_Out_PP;
	PC.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &PC);
}

//----------------------------------------------------------------------------

xQueueHandle Queue_temp; // объявляем дескриптор очереди

//----------------------------------------------------------------------------
void vBlinker1(void *pvParameters)
{
	while(true)
	{
		GPIOC->ODR ^= GPIO_Pin_13;
		vTaskDelay(500);
	}
}

//----------------------------------------------------------------------------
void vBlinker2(void *pvParameters)
{
	while(true)
	{
		GPIOC->ODR ^= GPIO_Pin_14;
		vTaskDelay(300);
	}
}
//----------------------------------------------------------------------------

void vTempMeasure(void *pvParameters)
{
	Ds18b20 ds18b20(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA); //создаем объект класса и инициализируем
	unsigned char crc_calc, crc_from_device; 
	float temper;
	while(true)
	{
		do
		{
			temper = ds18b20.getFTemperature();
			crc_from_device = ds18b20.getCRCFromDevice();
			crc_calc = ds18b20.calculateCrc();
			}while(crc_calc != crc_from_device);// сравниваем crc с датчика и crc вычисленный нами
		
		xQueueSendToBack(Queue_temp, &temper, 0);// пихаем температуру в очередь
			vTaskDelay(3000);// блок на 3 сек
	}
}

//----------------------------------------------------------------------------
void vDisplayTemp(void *pvParameters)
{
	while(true)
	{
		float temper;
		xQueueReceive(Queue_temp, &temper, 3000); // принимаем из очереди float, если очередь пуста уходим в блок на 3 сек
		floatToLCD(temper); // выводим на дисплей
		vTaskDelay(3000); // блок на 3 сек
	}
}
//----------------------------------------------------------------------------

int main()
{
		SystemCoreClockUpdate();
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    
		LedsInit();
	  TIM2_init();
		LCD_pinsInit();
	
		LCD_init();
		
//		Ds18b20 ds18b20(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA);
//		float temper = ds18b20.getFTemperature();
//		unsigned char crc = ds18b20.getCRCFromDevice();
//		unsigned char crc1 = ds18b20.calculateCrc();
	
		
		Queue_temp = xQueueCreate(5, sizeof(float));// очередь в 5 элементов размером типа float
	
		if(Queue_temp != NULL) // если удалось создать дескриптор очереди, запускаем планировщик
		{
			xTaskCreate(vBlinker1, "Blinker1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);//мигание светодиода низкийй приоритет
			xTaskCreate(vBlinker2, "Blinker2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);//мигание светодиода низкийй приоритет
			xTaskCreate(vTempMeasure, "temp", 2000, NULL, tskIDLE_PRIORITY + 3, NULL);// измерение температуры высокий приоритет
			xTaskCreate(vDisplayTemp, "lcd", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);// отображение температуры средний приоритет
			vTaskStartScheduler(); // поехали
		}
		else
			GPIOC->ODR |= GPIO_Pin_14;

}
