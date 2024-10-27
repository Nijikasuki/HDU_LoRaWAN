#include "sensors_test.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "stdio.h"
#include "rtc.h"
#include "stm32l4xx_hal.h"

int8_t Error_num = 0;

void Test_task(void)
{
    HDC1000_Test();
    OPT3001_Test();
    MPL3115_Test();
    MMA8451_Test();
    LORA_NODE_Test();
}

void HDC1000_Test(void)
{
    uint16_t temper, humi;

    temper = HDC1000_Read_Temper();
    humi = HDC1000_Read_Humidi();
		
    if(Error_num == 0)
    {
        if(temper !=0 || humi!=0)
        {
            debug_printf("温湿度传感器正常 温度: %.3f ℃   湿度: %.3f%% \r\n",(double)temper/1000.0,(double)humi/1000.0);
        } 
				else
        {
            Error_num = -13;
            debug_printf("温湿度传感器异常  error:%d \r\n",Error_num);
        }
    } 
		else
    {

        debug_printf("温湿度传感器异常  error:%d \r\n",Error_num);
    }

    Error_num = 0;
}


double HDC1000_return(void)
{
    uint16_t temper;
    temper = HDC1000_Read_Temper();
		double double_temp;
		double_temp = (double)temper/1000.0;
		return double_temp;
}

char* Get_RTC_Timestamp(void)
{
    static char timestamp[20];  // 用来存储时间戳的字符串

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // 获取当前时间和日期
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  // 获取日期，防止RTC内部状态错误

    // 格式化时间戳为 "YYYY-MM-DD HH:MM:SS"
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d",
             2000 + sDate.Year, sDate.Month, sDate.Date,
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    return timestamp;
}

int Get_RTC_Timestamp_lora_year(void)
{
    static int year;  

    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  // 获取日期，防止RTC内部状态错误
        
        year = sDate.Year;

    return year;
}

int Get_RTC_Timestamp_lora_month(void)
{
    static int month;  

    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  // 获取日期，防止RTC内部状态错误
        
        month = sDate.Month;

    return month;
}

int Get_RTC_Timestamp_lora_date(void)
{
    static int date;  

    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  // 获取日期，防止RTC内部状态错误
        
        date = sDate.Date;

    return date;
}


void OPT3001_Test(void)
{
    float lux;
    uint16_t result;

    result = OPT3001_Result();

    lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);

    if(Error_num == 0)
    {
        if(lux !=0 )
        {
            debug_printf("照度传感器正常 照度: %.2f Lux \r\n",lux);
        } else
        {
            Error_num = -16;
            debug_printf("照度传感器异常  error:%d \r\n",Error_num);
        }
    } else
    {
        debug_printf("照度传感器异常  error:%d \r\n",Error_num);
    }

    Error_num = 0;

}

void MPL3115_Test(void)
{
    float pressure;

    pressure = MPL3115_ReadPressure();

    if(Error_num == 0)
    {
        if(pressure !=0 )
        {
            debug_printf("气压传感器正常 气压: %.2f Pa \r\n",pressure);
        } else
        {
            Error_num = -18;
            debug_printf("气压传感器异常  error:%d \r\n",Error_num);
        }
    } else
    {
        debug_printf("气压传感器异常  error:%d \r\n",Error_num);
    }

    Error_num = 0;
}


void MMA8451_Test(void)
{
    ACCELER_T tAccel;

    tAccel.accel_x = 999;
    tAccel.accel_y = 999;
    tAccel.accel_z = 999;

    tAccel = MMA8451_ReadAcceleration();

    if(Error_num == 0)
    {
        if(tAccel.accel_x !=999 || tAccel.accel_y !=999)
        {
            debug_printf("加速度传感器正常 X: %d  Y: %d  Z: %d  \r\n",tAccel.accel_x,tAccel.accel_y,tAccel.accel_z);
        } else
        {
            Error_num = -20;
            debug_printf("加速度传感器异常  error:%d \r\n",Error_num);
        }
    } else
    {
        debug_printf("加速度传感器异常  error:%d \r\n",Error_num);
    }

    Error_num = 0;

}

void LORA_NODE_Test(void)
{
#if 0
    GET_LoraNode_Info(LoRa_Node_str);

    Printf_LoRaNode_Info(LoRa_Node);

    LoraNode_SetGPIO(255,0);
    HAL_Delay(1000);
    LoraNode_SetGPIO(255,1);
    LoraNode_Save();
#endif
}

