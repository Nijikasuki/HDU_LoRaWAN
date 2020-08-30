#include "key.h"
#include "gpio.h"
#include "usart.h"
#include "lowpower.h"

KEY App_Key;
KEY *App_Key_str = &App_Key;
extern DEVICE_SATE *Device_Sate_str;
extern uint8_t cmdwakeflg;
extern Node_Info *LoRa_Node_str;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(Gpio_KEY2,Pin_KEY2) == 0)
	{
		if(*Device_Sate_str == START_MODE)
		{
			debug_printf("--> Tips: Please select K1 first, switch to command mode!\r\n");
			return;
		}
		else//左边按键K2设置参数
		{ 
			HAL_Delay(300);  //保证按键中断以及busy中断不会同时触发，以免出现问题
			HAL_GPIO_TogglePin(Gpio_WAKE,Pin_WAKE);  //WAKE引脚翻转
			if(LORANODE_WAKE_STATUS)
				usart2_send_string("--> Module is awakened!\r\n");
			else usart2_send_string("--> Module goes to sleep!\r\n");
			if((*Device_Sate_str == CMD_MODE) && (LoRa_Node_str->Join == 0))
				cmdwakeflg = 1;
 		}
	}
	
	if(HAL_GPIO_ReadPin(Gpio_KEY1,Pin_KEY1) == 0)			             //右边按键K1开始通讯
 	{
		if(*Device_Sate_str != LPTSEND_MODE)
		{
			HAL_Delay(200);
	  		App_Key.A_KEY1 = 1;
		}
		else
        {
            App_Key.A_KEY1 = 1;  //stop模式下系统时钟停止运行，系统延时函数（HAL_Delay()）无法使用
        }
		KEY_DO(App_Key_str,Device_Sate_str);
	} 
	
	if(HAL_GPIO_ReadPin(Gpio_WAKE,Pin_WAKE) == 1)                       //WAKE pull high
	{
		HAL_GPIO_WritePin(LedGpio_WAKE, LedPin_WAKE, GPIO_PIN_RESET);   //D6 lighting
	}
	else
	{
		HAL_GPIO_WritePin(LedGpio_WAKE, LedPin_WAKE, GPIO_PIN_SET);     //D6 lights off
	}
	
	if(HAL_GPIO_ReadPin(Gpio_MODE,Pin_MODE) == 1)                      //MODE pull high
	{
		HAL_GPIO_WritePin(LedGpio_MODE, LedPin_MODE, GPIO_PIN_RESET);  //D7 lighting 
	}
	else
	{
		HAL_GPIO_WritePin(LedGpio_MODE, LedPin_MODE, GPIO_PIN_SET);   //D7 lights off
	}
	
	if(HAL_GPIO_ReadPin(Gpio_BUSY,Pin_BUSY) == 1)                       //BUSY pull high
 	{
		HAL_GPIO_WritePin(LedGpio_BUSY, LedPin_BUSY, GPIO_PIN_SET);     //D11lights off
	}
	else
	{
		HAL_GPIO_WritePin(LedGpio_BUSY, LedPin_BUSY, GPIO_PIN_RESET);   //D11 lighting 
	}
	
	if(HAL_GPIO_ReadPin(Gpio_STAT,Pin_STAT) == 1)                       //state pull high
 	{
		HAL_GPIO_WritePin(LedGpio_STAT, LedPin_STAT, GPIO_PIN_RESET);   //D8 lignting
	}
	else
	{
		HAL_GPIO_WritePin(LedGpio_STAT, LedPin_STAT, GPIO_PIN_SET);     //D8 lights off
	} 
}

void KEY_DO(KEY *key_temp,DEVICE_SATE *Device_Sate_temp)
{
	if(key_temp->A_KEY1 == 1 )
	{
		switch(*Device_Sate_temp)
		{		
			case START_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;

			case CMD_MODE:
				*Device_Sate_temp = TRANSPARENT_MODE;
				break;
            
			case TRANSPARENT_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
            case LPTSEND_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
                
			case TESTAUTO_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
                
            case TIMSEND_MODE:
                *Device_Sate_temp = CMD_MODE;
            break;
		}
		key_temp->A_KEY1 = 0;	
	}
}


