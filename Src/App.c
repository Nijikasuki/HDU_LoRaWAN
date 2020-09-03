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
	
	lr_borad_information_print();    //ʹ��˵��	
	switch((uint8_t)device_func)
	{
		/* ָ�����ù��� */
		case CMD_CONFIG_FUNC:  			
		{	
			/* �������command Configuration function, �����ú���,ִֻ��һ�� */
			if(dev_stat != CMD_CONFIG_FUNC)
			{
				dev_stat = CMD_CONFIG_FUNC;
				debug_printf("Now is command configuration function.\r\n");
				node_gpio_set(wake, wakeup);
				node_gpio_set(mode, command);
			}
			/* �ȴ�usart2�����ж� */
			if(Usart2_RX.receive_flag)
			{
				Usart2_RX.receive_flag = 0;
				lpusart1_send_data(Usart2_RX.RX_Buf,Usart2_RX.rx_len);
			} 
			/* �ȴ�lpuart1�����ж� */
			if(LPUsart1_RX.receive_flag)
			{
				LPUsart1_RX.receive_flag = 0;
				usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			}
		}
		break;
		
		/* ���ݴ��书�� */
		case DATA_TRANSPORT_FUNC:
		{   
			/* �������data transport function,�����ú���,ִֻ��һ�� */
			if(dev_stat != DATA_TRANSPORT_FUNC)
			{
				dev_stat = DATA_TRANSPORT_FUNC;
				debug_printf("Now is data transport function.\r\n");
				
				/* ģ�������ж� */
				if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
				{
					return;
				}
			}
			/* �ȴ�usart2�����ж� */
			if(Usart2_RX.receive_flag && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
            {
				Usart2_RX.receive_flag = 0;
				send_result = nodeDataCommunicate((uint8_t*)Usart2_RX.RX_Buf, Usart2_RX.rx_len, &pphead);
			}
			/* ���ģ����æ, ����������Ч��������������Ϣ */
			else if(Usart2_RX.receive_flag && (LORANODE_BUSY_STATUS == 0))
            {
                Usart2_RX.receive_flag = 0;
                debug_printf("--> Warning: Don't send data now! The modem is busy!\r\n");
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
void lr_borad_information_print(void)
{
	PRINT_ATE_VERSION_INFO("%s",ATE_VERSION);
	debug_printf("--> 1  Data reporting using button Key1: \r\n");
	debug_printf("--> 2 function of Data reporting regularly using button Key2: \r\n");
	LEDALL_ON;
	HAL_Delay(100);
	LEDALL_OFF;
}




