#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"
#include "sensors_test.h"


extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str; 
down_list_t *pphead = NULL;

//-----------------Users application--------------------------
void lorawan_func_process(void)
{
	static DEVICE_MODE_T dev_stat = NO_MODE;
	
	uint16_t temper = 0;

	switch((uint8_t)device_mode)
	{
		/* 指令配置功能 */
		case CMD_CONFIG_MODE:  			
		{	
			/* 如果不是command Configuration function, 则进入if语句,只执行一次 */
			if(dev_stat != CMD_CONFIG_MODE)
			{
				dev_stat = CMD_CONFIG_MODE;
				debug_printf("\r\nNow is command configuration mode.\r\n");
				
				nodeGpioConfig(wake, wakeup);
				nodeGpioConfig(mode, command);
			}
			/* 等待usart2产生中断 */
			if(UART_TO_PC_RECEIVE_FLAG)
			{
				UART_TO_PC_RECEIVE_FLAG = 0;
				lpusart1_send_data(UART_TO_PC_RECEIVE_BUFFER,UART_TO_PC_RECEIVE_LENGTH);
			} 
			/* 等待lpuart1产生中断 */
			if(UART_TO_LRM_RECEIVE_FLAG)
			{
				UART_TO_LRM_RECEIVE_FLAG = 0;
				usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
			}
		}
		break; 
		
		/* 数据传输功能 */
		case DATA_TRANSPORT_MODE:
		{   
			/* 如果不是data transport function,则进入if语句,只执行一次 */
			if(dev_stat != DATA_TRANSPORT_MODE)
			{
				dev_stat = DATA_TRANSPORT_MODE;
				debug_printf("\r\nNow is data transport mode.\r\n");
				
				/* 模块入网判断 */
				if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
				{
					return;
				} 
				
				temper = HDC1000_Read_Temper()/1000;
				
				nodeDataCommunicate((uint8_t*)&temper,sizeof(temper),&pphead);				
			}
			
			/* 等待usart2产生中断 */
			if(UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
            {
				UART_TO_PC_RECEIVE_FLAG = 0;
				nodeDataCommunicate((uint8_t*)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
			}
			
			/* 如果模块正忙, 则发送数据无效，并给出警告信息 */
			else if(UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
            {
                UART_TO_PC_RECEIVE_FLAG = 0;
                debug_printf("--> Warning: Don't send data now! The module is busy!\r\n");
            }
			
			/* 等待lpuart1产生中断 */
			if(UART_TO_LRM_RECEIVE_FLAG)
			{
				UART_TO_LRM_RECEIVE_FLAG = 0;
				usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
			}
		} 
		break;
		
		/*Class C数据下行功能*/
		case PRO_TRAINING_MODE:
		{      
			/* 如果不是Class C云平台数据采集模式, 则进入if语句,只执行一次 */
			if(dev_stat != PRO_TRAINING_MODE)
			{
				dev_stat = PRO_TRAINING_MODE;
				debug_printf("\r\nNow is project training mode.\r\n");
				
				/* 模块入网判断 */
				if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
				{
					return;
				} 
			}
			
			/* 如果LPUART1有下行数据 */
			if(UART_TO_LRM_RECEIVE_FLAG)
			{
				/* 清除串口接收标志 */
				UART_TO_LRM_RECEIVE_FLAG = 0;
				
				/* 匹配字符"ZZAA" */
				if(strstr((char*)UART_TO_LRM_RECEIVE_BUFFER,"ZZAA"))
				{
					temper = HDC1000_Read_Temper()/1000;
					nodeDataCommunicate((uint8_t*)&temper, sizeof(temper), &pphead);
				}
			}
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
void lorawan_borad_infor_print(void)
{
	debug_printf("\r\n\r\n");
	PRINT_CODE_VERSION_INFO("%s",CODE_VERSION);
	debug_printf("\r\n");
	debug_printf("--> 1 Using button Key1 can: \r\n");
	debug_printf("-->   1.1 enter command Mode\r\n");
	debug_printf("-->   1.2 enter transparent Mode\r\n");
	debug_printf("--> 2 Using button Key2 can: \r\n");
	debug_printf("-->   2.1 entering project trainning Mode\r\n");
	LEDALL_ON;
	HAL_Delay(100);
	LEDALL_OFF;
}




