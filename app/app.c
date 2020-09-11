#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"

extern TIM_HandleTypeDef TIM6_InitStruct;


DEVICE_FUNCTION device_func = NO_FUNC;
DEVICE_FUNCTION *Device_Sate_str = &device_func; 

extern uint8_t at_id;


//-----------------Users application--------------------------
void lorawan_func_process(void)
{
	static DEVICE_FUNCTION dev_stat = NO_FUNC;
	down_list_t *pphead = NULL;
	uint16_t data_temperature;
	char data_hex_test[] = {0x0A,0x02,0x03,0x04,0x05};
	
	switch((uint8_t)device_func)
	{
		/* ָ�����ù��� */
		case CMD_CONFIG_FUNC:  			
		{	
			/* �������command Configuration function, �����if���,ִֻ��һ�� */
			if(dev_stat != CMD_CONFIG_FUNC)
			{
				dev_stat = CMD_CONFIG_FUNC;
				debug_printf("\r\nNow is command configuration function.\r\n");
				
				nodeGpioConfig(wake, wakeup);
				nodeGpioConfig(mode, command);
			}
			/* �ȴ�usart2�����ж� */
			if(USART2_RECEIVE_FLAG)
			{
				USART2_RECEIVE_FLAG = 0;
				lpusart1_send_data(USART2_RECEIVE_BUFFER,USART2_RECEIVE_LENGTH);
			} 
			/* �ȴ�lpuart1�����ж� */
			if(LPUART1_RECEIVE_FLAG)
			{
				LPUART1_RECEIVE_FLAG = 0;
				usart2_send_data(LPUART1_RECEIVE_BUFFER,LPUART1_RECEIVE_LENGTH);
			}
		}
		break; 
		
		/* ���ݴ��书�� */
		case DATA_TRANSPORT_FUNC:
		{   
			/* �������data transport function,�����if���,ִֻ��һ�� */
			if(dev_stat != DATA_TRANSPORT_FUNC)
			{
				dev_stat = DATA_TRANSPORT_FUNC;
				debug_printf("\r\nNow is data transport function.\r\n");
				
				/* ��ȡһ�ε�ǰ�¶� */
				data_temperature = HDC1000_Read_Temper();
				data_temperature = data_temperature/1000;
				
				/* ģ�������ж� */
				if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
				{
					return;
				} 
				/* ģ�������¶ȴ��������� */
				nodeDataCommunicate((uint8_t*)&data_temperature, strlen((char*)&data_temperature), &pphead);
//				debug_printf(" pphead->down_info.business_data = %s, pphead->down_info.size = %d\r\n",LPUART1_RECEIVE_BUFFER,LPUART1_RECEIVE_LENGTH);
//				debug_printf(" pphead->down_info.business_data = %s, pphead->down_info.size = %d\r\n",pphead->down_info.business_data,pphead->down_info.size);
			}
			/* �ȴ�usart2�����ж� */
			if(USART2_RECEIVE_FLAG && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
            {
				USART2_RECEIVE_FLAG = 0;
				nodeDataCommunicate((uint8_t*)USART2_RECEIVE_BUFFER, USART2_RECEIVE_LENGTH, &pphead);
			}
			/* ���ģ����æ, ����������Ч��������������Ϣ */
			else if(USART2_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
            {
                USART2_RECEIVE_FLAG = 0;
                debug_printf("--> Warning: Don't send data now! The module is busy!\r\n");
            }
		} 
		break;
		
		/*Class C�������й���*/
		case DATA_REPORT_ONE_TIME_FUNC:
		{      
			
		} 
		break;
		
		/* ���������ϱ����� */
		case DATA_REGU_REPORT_FUNC:
		{
			
		}
		break;
		
		default: break;
	}
}


/**
 * @brief   ������汾��Ϣ�Ͱ���ʹ��˵����Ϣ��ӡ
 * @details �ϵ����еƻ������100ms
 * @param   ��
 * @return  ��
 */
void lorawan_borad_infor_print(void)
{
	debug_printf("\r\n\r\n");
	PRINT_CODE_VERSION_INFO("%s",CODE_VERSION);
	debug_printf("--> 1  Data reporting using button Key1: \r\n");
	debug_printf("--> 2 function of Data reporting regularly using button Key2: \r\n");
	LEDALL_ON;
	HAL_Delay(100);
	LEDALL_OFF;
}




