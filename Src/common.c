#include "stm32l4xx.h"
#include "common.h"

uint8_t TimeOut_Sign = 0;


uint8_t Time_Out_Break(uint32_t MAX_time)
{
	static uint32_t time_start = 0;
	static uint32_t time_new = 0;
	uint16_t temp=0;

	if(TimeOut_Sign == 0)
	{
		TimeOut_Sign = 1;
		time_start = HAL_GetTick();
	}
	if(TimeOut_Sign == 1)
	{
		time_new = HAL_GetTick();
		temp = time_new - time_start;
		if(temp > MAX_time)
		{
			TimeOut_Sign = 0;
			return 1;
		}
		else
		{return 0;}
	}
	return 0;
}
