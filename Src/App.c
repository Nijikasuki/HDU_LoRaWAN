#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "str.h"
#include "lorawan.h"
#include "lowpower.h"
#include "atcmd.h"
#include "tim.h"
#include "lsd_lorawan_driver.h"


extern TIM_HandleTypeDef TIM6_InitStruct;

extern usart_recv_t Usart2_RX; 
extern usart_recv_t LPUsart1_RX;

DEVICE_FUNCTION device_func = NO_FUNC;
DEVICE_FUNCTION *Device_Sate_str = &device_func; 

extern Node_Info *LoRa_Node_str;
extern Node_Info LoRa_Node;

extern Send_Info *LoRa_Send_str;
extern Send_Info LoRa_Send;

extern uint8_t at_id;


//-----------------Users application--------------------------
void lr_func_achive(void)
{
	static DEVICE_FUNCTION dev_stat = 0;
	execution_status_t send_result;
	down_list_t *pphead = NULL;
	
	lr_borad_information_print();    //使用说明	
	switch((uint8_t)device_func)
	{
		/* 指令配置功能 */
		case CMD_CONFIG_FUNC:  			
		{	
			/* 如果不是command Configuration function, 则进入该函数,只执行一次 */
			if(dev_stat != CMD_CONFIG_FUNC)
			{
				dev_stat = CMD_CONFIG_FUNC;
				debug_printf("Now is command configuration function.\r\n");
				node_gpio_set(wake, wakeup);
				node_gpio_set(mode, command);
			}
			/* 等待usart2产生中断 */
			if(Usart2_RX.receive_flag)
			{
				Usart2_RX.receive_flag = 0;
				lpusart1_send_data(Usart2_RX.RX_Buf,Usart2_RX.rx_len);
			} 
			/* 等待lpuart1产生中断 */
			if(LPUsart1_RX.receive_flag)
			{
				LPUsart1_RX.receive_flag = 0;
				usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			}
		}
		break;
		
		/* 数据传输功能 */
		case DATA_TRANSPORT_FUNC:
		{   
			/* 如果不是data transport function,则进入该函数,只执行一次 */
			if(dev_stat != DATA_TRANSPORT_FUNC)
			{
				dev_stat = DATA_TRANSPORT_FUNC;
				debug_printf("Now is data transport function.\r\n");
				
				/* 模块入网判断 */
				if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
				{
					return;
				}
			}
			/* 等待usart2产生中断 */
			if(Usart2_RX.receive_flag && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
            {
				Usart2_RX.receive_flag = 0;
				send_result = nodeDataCommunicate((uint8_t*)Usart2_RX.RX_Buf, Usart2_RX.rx_len, &pphead);
			}
			/* 如果模块正忙, 则发送数据无效，并给出警告信息 */
			else if(Usart2_RX.receive_flag && (LORANODE_BUSY_STATUS == 0))
            {
                Usart2_RX.receive_flag = 0;
                debug_printf("--> Warning: Don't send data now! The modem is busy!\r\n");
            }
		} 
		break;
		
		/*Class C数据下行功能*/
		case DATA_REPORT_ONE_TIME_FUNC:
		{      
			
		} 
		break;
		
		/* 数据周期上报功能 */
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




