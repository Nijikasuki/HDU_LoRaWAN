#include "key.h"
#include "gpio.h"
#include "usart.h"
#include "lowpower.h"

KEY App_Key;
KEY *App_Key_str = &App_Key;
extern DEVICE_FUNCTION *Device_Sate_str;
extern uint8_t cmdwakeflg;
extern Node_Info *LoRa_Node_str;


/**
 * @brief		MCU�����жϻص�����
 * @details  	�ú�������������2�������Լ���lorawanģ������4������mode\wake\busy\stat���жϴ���
 * @param    	GPIO_Pin:stm32ֱ�ӵ��ã����账��
 * @return   	��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GET_KEY1_LEVEL == GPIO_PIN_RESET && GET_KEY2_LEVEL == GPIO_PIN_RESET)
		return;
	if(GET_KEY1_LEVEL == GPIO_PIN_RESET)			             //�ұ߰���K1��ʼͨѶ
 	{
		App_Key.A_KEY1 = 1;
	} 
	if(GET_KEY2_LEVEL == GPIO_PIN_RESET)
	{
		App_Key.A_KEY2 = 1;
	}
	HAL_Delay(200);
	KEY_DO(App_Key_str,Device_Sate_str);
	
	/*-- ģ��mode\wake\busy\stat״ָ̬ʾ --*/
	(GET_LED_WAKE_LEVEL == GPIO_PIN_SET) ? LEDWAKE_ON : LEDWAKE_OFF;                    
	
	(GET_LED_MODE_LEVEL == GPIO_PIN_SET) ? LEDMODE_ON : LEDMODE_OFF;                     
	
	(GET_LED_BUSY_LEVEL == GPIO_PIN_SET) ? LEDBUSY_OFF : LEDBUSY_ON;                      

	(GET_LED_STAT_LEVEL == GPIO_PIN_SET) ? LEDSTAT_ON :  LEDSTAT_OFF;                    
}


/**
 * @brief		����������
 * @param    	key_temp��key1 or key2, Device_Sate_temp:�豸����
 * @return   	��
 */
void KEY_DO(KEY *key_temp,DEVICE_FUNCTION *device_func_temp)
{
	if(key_temp->A_KEY1 == 1 )
	{
		key_temp->A_KEY1 = 0;	
		switch((uint8_t)*device_func_temp)
		{		
			case CMD_CONFIG_FUNC:
				*device_func_temp = DATA_REPORT_ONE_TIME_FUNC;
			break;

			case DATA_REPORT_ONE_TIME_FUNC:
				*device_func_temp = CMD_CONFIG_FUNC;
			break;
		}
	}
	else if(key_temp->A_KEY2 == 1)
	{
		key_temp->A_KEY2 = 0;	
		switch((uint8_t)*device_func_temp)
		{		
			case CMD_CONFIG_FUNC:
				*device_func_temp = DATA_REPORT_ONE_TIME_FUNC;
			break;
			
			case DATA_REGU_REPORT_FUNC:
				*device_func_temp = CMD_CONFIG_FUNC;
			break;
		}
	}		
}


