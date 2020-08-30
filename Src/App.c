#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "str.h"
#include "lorawan.h"
#include "lowpower.h"
#include "atcmd.h"
#include "tim.h"

extern uint8_t TimeOut_Sign;  //Timeout sign
uint8_t Send_Sate_Sign = 0;

int8_t send_log = 0; //send data log number

uint8_t cmdwakeflg = 0;//Command mode sleep flag
 
extern TIM_HandleTypeDef TIM6_InitStruct;

extern usart_recv_t Usart2_RX; 
extern usart_recv_t LPUsart1_RX;

DEVICE_FUNCTION device_func = POWER_ON_FUNC;
DEVICE_FUNCTION *Device_Sate_str = &device_func; 

extern Node_Info *LoRa_Node_str;
extern Node_Info LoRa_Node;

extern Send_Info *LoRa_Send_str;
extern Send_Info LoRa_Send;

extern uint8_t at_id;


//-----------------Users application--------------------------
void lr_func_achive(void)
{
	static DEVICE_FUNCTION dev_stat = 0;//Joindebug:Used to send join information
    char *Send_Context = "012345678998765432100123456789987654321001234567899876543210abcdefghijklmnopqrstuvwxyz";
    static uint8_t timsend_enter = 0;
    static uint32_t comm_total_data = 0, comm_succ_data = 0;
    uint32_t join_start_time = 0, join_end_time = 0;
    
	lr_borad_information_print();    //使用说明
	
	switch((uint8_t)device_func)
	{
		case POWER_ON_FUNC:   //接收PC发送的数据
		{
			if(Usart2_RX.receive_flag)
			{
				Usart2_RX.receive_flag = 0;
				debug_printf("--> Tips: Please select K1 first, switch to command mode!\r\n");
			}
		}
		break;
		
		case DATA_REPORT_ONE_TIME_FUNC:
		{      
			
		} 
		break;
		
		case DATA_REGU_REPORT_FUNC:
		{
			
		}
		break;
		
		default: break;
	}
}


/**
 * @brief   开发板版本信息和按键使用说明信息打印
 * @details 上电所有灯会短暂亮100ms
 * @param   无
 * @return  无
 */
void lr_borad_information_print(void)
{
	PRINT_ATE_VERSION_INFO("%s",ATE_VERSION);
	debug_printf("--> 1  Data reporting using button Key1: \r\n");
	debug_printf("--> 2 function of Data reporting regularly using button Key2: \r\n");
	LEDALL_ON;
	HAL_Delay(100);
	LEDALL_OFF;
}




