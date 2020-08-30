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

extern USART_RECEIVETYPE Usart2_RX; 
extern USART_RECEIVETYPE LPUsart1_RX;

DEVICE_SATE Device_Sate = START_MODE;
DEVICE_SATE *Device_Sate_str = &Device_Sate; 

extern Node_Info *LoRa_Node_str;
extern Node_Info LoRa_Node;

extern Send_Info *LoRa_Send_str;
extern Send_Info LoRa_Send;

extern uint8_t at_id;
//-----------------Users application--------------------------
void User_App(void)
{
	static uint8_t start = 0;//Joindebug:Used to send join information
    uint8_t *Send_Context = "012345678998765432100123456789987654321001234567899876543210abcdefghijklmnopqrstuvwxyz";
    static uint8_t timsend_enter = 0;
    static uint32_t comm_total_data = 0, comm_succ_data = 0;
    uint32_t join_start_time = 0, join_end_time = 0;
    
	switch((uint8_t)Device_Sate)
	{
		case START_MODE:
		{
			if(start != 1)
			{
				start = 1;
				debug_printf("************ ---LORAWAN_NODE Development board program V04.01 2020.01.02 --- ************\r\n");
				debug_printf("--> 1. Mode selection using button Key1: \r\n");
				debug_printf("-->   1.1 Command Mode\r\n");
				debug_printf("-->   1.2 Transparent Mode\r\n");
                debug_printf("--> 2. Sleep management using button Key2: \r\n");
				debug_printf("-->   2.1 Active state\r\n");
				debug_printf("-->   2.2 Sleep state\r\n");
                debug_printf("--> 3. In command mode,please inputting 'AT+LPTSEND' to enter low power timing transmission mode.\r\n");
				debug_printf("--> 4. In command mode,please inputting 'AT+TESTAUTO' to enter command automatic detection mode.\r\n");
                debug_printf("--> 5. In command mode,please inputting 'AT+TIMSEND' to enter timing transmission mode.\r\n");
				LEDALL_ON;
				HAL_Delay(100);
				LEDALL_OFF;            
            }
			if(Usart2_RX.receive_flag)
			{
				Usart2_RX.receive_flag = 0;
				debug_printf("--> Tips: Please select K1 first, switch to command mode!\r\n");
			}
		}
		break;
		case CMD_MODE:
		{      
			if(start != 2)
			{ 
				debug_printf("Now enter CMD Mode!\r\n");
				start = 2;
				LEDWAKE_ON;
				LEDMODE_ON;
				if(timsend_enter)
                {
                    timsend_enter = 0;
                    HAL_TIM_Base_Stop_IT(&TIM6_InitStruct);
                }
				LoRaNode_Wake_Sleep(MODE_WAKEUP);
				LoRaNode_Mode_Set(MODE_CMD);
               
                //test
//                LoRaNode_Wake_Sleep(MODE_SLEEP);
//                HAL_Delay(500);
//                HAL_GPIO_WritePin(Gpio_RST, Pin_RST, GPIO_PIN_RESET);
//                HAL_Delay(10);
//                HAL_GPIO_WritePin(Gpio_RST, Pin_RST, GPIO_PIN_SET);
//                HAL_Delay(200);
//                LoRaNode_Wake_Sleep(MODE_WAKEUP);
			}

			if(LPUsart1_RX.receive_flag)
			{
				LPUsart1_RX.receive_flag = 0;
				usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			}

			if(Usart2_RX.receive_flag && LORANODE_WAKE_STATUS)
			{
				Usart2_RX.receive_flag = 0;

				lowstr_num_to_uppstr(Usart2_RX.RX_Buf,Usart2_RX.rx_len);
				if(strnstr((char*)Usart2_RX.RX_Buf,"AT+TESTAUTO\r",Usart2_RX.rx_len))
				{
					Device_Sate = TESTAUTO_MODE;
					break;
				}
                else if(strnstr((char*)Usart2_RX.RX_Buf,"AT+LPTSEND\r",Usart2_RX.rx_len))
				{
					Device_Sate = LPTSEND_MODE;
					break;
				}
                else if(strnstr((char*)Usart2_RX.RX_Buf,"AT+TIMSEND\r",Usart2_RX.rx_len))
				{
					Device_Sate = TIMSEND_MODE;
					break;
				}
				lpusart1_send_data(Usart2_RX.RX_Buf,Usart2_RX.rx_len);
			}
			else if(Usart2_RX.receive_flag && (LORANODE_WAKE_STATUS == 0))
			{
				Usart2_RX.receive_flag = 0;
				debug_printf("--> Warning: Don't send data now! The modem is sleeping!\r\n");
			}
		} 
		break;
        
		case TRANSPARENT_MODE:
		{
			if(start != 3)
			{
				start = 3;
				LEDWAKE_ON;
				LEDMODE_OFF;
				
				LoRaNode_Wake_Sleep(MODE_WAKEUP);
				debug_printf("Now enter transparent mode!\r\n");
//				LoRaNode_Mode_Set(MODE_CMD);
//				LoRaNode_GetNetMode(LoRa_Node_str);
//				LoRaNode_GetConfirm(LoRa_Node_str);
//				LoRaNode_GetJoinState(LoRa_Node_str);
//				Joindebug = 1;
				/*This instruction is used to solve the problem that
				the module is hibernated in the command mode and 
				switched to transparent transmission without JOIN.
				*/
#if 0
				if(cmdwakeflg)
				{
					cmdwakeflg = 0;
					LoRaNode_Reset();
				}
#endif
				/* After the reset command is triggered, 
				 * an interrupt will be entered, and the MODE indicator will be in the wrong state.
				 * So the MODE indicator needs to be turned off again.
				 */
				LEDMODE_OFF;
				
                if(LoRaNode_JoinDetect() == 0)//join successful
                {
                }
            }
            if(Usart2_RX.receive_flag && (LORANODE_BUSY_STATUS)) //Ensure BUSY is high before sending data
            {
                Usart2_RX.receive_flag = 0;
                //debug_printf("--> The data has been sent to the module. Please do not send data before BUSY is pulled high.\r\n");
                send_log = LoRaNode_Write_Receive_Bytes(Usart2_RX.RX_Buf,Usart2_RX.rx_len,LoRa_Node.Confirm);
                if(send_log == 1)
                {
                    debug_printf("--> Data correspond success!\r\n");
                }
                if(send_log == -1)
                {
                    debug_printf("--> The module is busy!\r\n");
                }
                if(send_log == -2)
                {
                    debug_printf("--> The module did not receive the serial port data or module did not work!\r\n");
                }
                if(send_log == -3)
                {
                    debug_printf("--> The module sent timeout!\r\n");
                }
                if(send_log == -4)
                {
                    debug_printf("--> Data communication failure!\r\n");
                }
            }
            else if(Usart2_RX.receive_flag && (LORANODE_WAKE_STATUS == 0))
            {
                Usart2_RX.receive_flag = 0;
                debug_printf("--> Warning: Don't send data now! The modem is sleeping!\r\n");
            }
            else if(Usart2_RX.receive_flag && (LORANODE_BUSY_STATUS == 0))
            {
                Usart2_RX.receive_flag = 0;
                debug_printf("--> Warning: Don't send data now! The modem is abnormal!\r\n");
            }
			if(LPUsart1_RX.receive_flag == 1)
			{
				LPUsart1_RX.receive_flag = 0;
				debug_printf("--> Debug log:");
				usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			}
		}
		break;
        case LPTSEND_MODE:
            {
                if(start != 4)
                {
                    start = 4;
                    LEDWAKE_ON;
                    LEDMODE_OFF;
                    debug_printf("Now is entering low power timing transmission mode!\r\n");
                    debug_printf("Data will start to be sent after 3s!\r\n");
                    HAL_Delay(3000);
                    LoRaNode_Wake_Sleep(MODE_WAKEUP);
                    join_start_time = HAL_GetTick();
                    if(LoRaNode_JoinDetect() != 0)
                    {
                        return;
                    }
                    join_end_time = HAL_GetTick();
                    if((join_end_time - join_start_time) > 5000) //再次JOIN，计数清0
                    {
                        comm_total_data = 0;
                        comm_succ_data = 0;
                    }
                }
                if(Device_Sate == CMD_MODE)
                {
                    break;
                }
//                if(comm_total_data %10 != 0) 
//                {
//                    if(StringStr(Operate_Atcmd("AT+CONFIRM","=0,1","\0"),RETURN_FALSE) != 0)
//                    {
//                        Operate_Atcmd("AT+CONFIRM","=0,1","\0");
//                    }
//                }  
//                else
//                {
//                    if(StringStr(Operate_Atcmd("AT+CONFIRM","=1,1","\0"),RETURN_FALSE) != 0)
//                    {
//                        Operate_Atcmd("AT+CONFIRM","=1,1","\0");
//                    }
//                }
                    
                send_log = LoRaNode_Write_Receive_Bytes(Send_Context,strlen((const char*)Send_Context),LoRa_Node.Confirm);
                comm_total_data ++;
                debug_printf("\r\n");
                Operate_Atcmd("AT+STATUS","?","\0");
                if(send_log == 1)
                {
                    comm_succ_data ++;
                    debug_printf("--> Data correspond success!\r\n");
                }
                if(send_log == -1)
                {
                    debug_printf("--> The module is busy!\r\n");
                }
                if(send_log == -2)
                {
                    debug_printf("--> The module did not receive the serial port data or module did not work!\r\n");
                }
                if(send_log == -3)
                {
                    debug_printf("--> The module sent timeout!\r\n");
                }
                if(send_log == -4)
                {
                    debug_printf("--> Data communication failure!\r\n");
                }
                debug_printf("--> Total pac_num is %d, fail pac_num is %d, success rate is %.1f%% \r\n",\
                             comm_total_data,comm_total_data-comm_succ_data,((double)comm_succ_data*100)/((double)comm_total_data));
                LoRaNode_Go_To_Sleep(RTC_Wakeup_Time);
            }
        break;
		case TESTAUTO_MODE:
		{
			if(start != 5)
			{
				start = 5;
				at_id = 1;
				debug_printf("Now is entering automatic detection Mode!\r\n");
				debug_printf("Automatic detection of instructions after 3s!\r\n");
				HAL_Delay(3000);
				Atcmd_AutoDect();
				HAL_Delay(1000);
				Device_Sate = CMD_MODE;
			}
		}
		break;
        case TIMSEND_MODE:
            {
                if(start != 6)
                {
                    start = 6;
                    LEDWAKE_ON;
                    LEDMODE_OFF;
                    timsend_enter = 1;
                    debug_printf("Now is entering timing transmission mode!\r\n");
                    debug_printf("Data will start to be sent after 3s!\r\n");
                    HAL_Delay(3000);
                    LoRaNode_Wake_Sleep(MODE_WAKEUP);
                    if(LoRaNode_JoinDetect() != 0)
                    {
                        return;
                    }
                    Tim6_Conf(5000);                      //计数器中断时间：5s
                    HAL_TIM_Base_Start_IT(&TIM6_InitStruct);
                }
                if(Device_Sate == CMD_MODE)
                {
                    break;
                }
                if(LPUsart1_RX.receive_flag == 1)
                {
                    LPUsart1_RX.receive_flag = 0;
                    usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
                }
            }
        break;
		default: break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t timing_ssend_cnt = 0,timing_hsend_cnt  = 0 ;
    uint8_t *Send_Context = "11223344556677889900";
    timing_ssend_cnt ++;
    if(timing_ssend_cnt == 6)        //30s
    {
        timing_ssend_cnt = 0;
        timing_hsend_cnt ++;
        if(timing_hsend_cnt == 1)    //30s
        {
            timing_hsend_cnt = 0;
            send_log = LoRaNode_Write_Receive_Bytes(Send_Context,strlen((const char*)Send_Context),LoRa_Node.Confirm);
            if(send_log == 1)
            {
                debug_printf("--> Data correspond success!\r\n");
            }
            if(send_log == -1)
            {
                debug_printf("--> The module is busy!\r\n");
            }
            if(send_log == -2)
            {
                debug_printf("--> The module did not receive the serial port data or module did not work!\r\n");
            }
            if(send_log == -3)
            {
                debug_printf("--> The module sent timeout!\r\n");
            }
            if(send_log == -4)
            {
                debug_printf("--> Data communication failure!\r\n");
            }
        }
        
    }  
}





