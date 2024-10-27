#ifndef _SENSORS_TEST_H_
#define _SENSORS_TEST_H_

#include "stm32l4xx_hal.h"


void Test_task(void);
void HDC1000_Test(void);
void OPT3001_Test(void);
void MPL3115_Test(void);
void MMA8451_Test(void);
void LORA_NODE_Test(void);
double HDC1000_return(void);
char* Get_RTC_Timestamp(void);
int Get_RTC_Timestamp_lora_year(void);
int Get_RTC_Timestamp_lora_month(void);
int Get_RTC_Timestamp_lora_date(void);
#endif

