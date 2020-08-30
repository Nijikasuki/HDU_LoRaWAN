#include <string.h>
#include "atcmd.h"
#include "lorawan.h"
#include "app.h"
#include "lowpower.h"
#include "str.h"
#include "usart.h"
extern int8_t send_log;  
extern uint8_t AT_Data_buf[RXLEN];
extern uint8_t TimeOut_Sign;
extern USART_RECEIVETYPE LPUsart1_RX;
extern Node_Info LoRa_Node;
extern Node_Info *LoRa_Node_str;
uint8_t at_id = 1, at_fail_id[ATCMD_MAX_FAILCOUNT] = {0},at_fail_id_count = 0;//atcmd id,at fail id
uint8_t *p_atcmd_type = "\0",*p_atcmd_set = "\0",*p_atcmd_back = "\0";//p_atcmd_type:ָ������; p_atcmd_set��ָ������;p_atcmd_back:ָ���ֵ
uint8_t at_success_count = 0,at_failure_count = 0;//ָ��ɹ��������ָ��ʧ�������
uint8_t equal_symbol = '=', inquiry_symbol = '?';
uint8_t inquiry_res[256] = {0};//inquiry result
uint8_t *enter_wrap = "\r\n";
uint8_t *atcmd_failure[64] = {0};

extern DEVICE_SATE Device_Sate;

//ָ����Ϻ���
//���ܣ�һ������ָ���Ϊ"AT+XX" = (or ?) "param" ��Ȼ��ͬ����Ͼ��γ��˲�ͬ�ĺ���
void Atcmd_Combination(ATCMD_TYPE at_type, uint8_t atcmd_number)
{
	switch((uint8_t)at_type)
	{
    case FACTORY_CMD://common atcmd
        {  
            switch(atcmd_number)
            {
            case 0:
                p_atcmd_type = "AT+FACTORY",p_atcmd_set = "\0",p_atcmd_back ="\0";
                break;
//            case 1:
//                p_atcmd_type = "AT+RX1",p_atcmd_set = "=1000,1",p_atcmd_back ="\0";
//                break;    
            case 2:
                p_atcmd_type = "AT+BAUD",p_atcmd_set = "?",p_atcmd_back ="9600";
                break;  
            case 3:
                p_atcmd_type = "AT+timeout",p_atcmd_set = "?",p_atcmd_back ="0, 0";
                break;
            case 4:
                p_atcmd_type = "AT+system",p_atcmd_set = "?",p_atcmd_back ="0";
                break;
            case 5:
                p_atcmd_type = "AT+DEBUG",p_atcmd_set = "?",p_atcmd_back ="0";
                break;
            case 6:
                p_atcmd_type = "AT+OTAA",p_atcmd_set = "?",p_atcmd_back ="1, 0, 1, 1";
                break;
            case 7:
                p_atcmd_type = "AT+CLASS",p_atcmd_set = "?",p_atcmd_back = "0";
                break;
            case 8:
//                p_atcmd_type = "AT+DATARATE",p_atcmd_set = "?",p_atcmd_back = "3, 3, 50, 08, 20";
                break;
            case 9:
                p_atcmd_type = "AT+CONFIRM",p_atcmd_set = "?",p_atcmd_back = "0, 4";
                break;
            case 10:
                p_atcmd_type = "AT+ADR",p_atcmd_set = "?",p_atcmd_back = "1, 64, 32, 0, 16384";
                break;
            case 11:
                p_atcmd_type = "AT+RX1",p_atcmd_set = "?",p_atcmd_back = "1000, 0, 5000";
                break;
            case 12:
                p_atcmd_type = "AT+POWER",p_atcmd_set = "?",p_atcmd_back = "0";
                break;
            case 13:
                p_atcmd_type = "AT+RTC",p_atcmd_set = "?",p_atcmd_back = "18 01 01";
                break;  
//            case 14:
//                p_atcmd_type = "AT+RX2",p_atcmd_set = "?",p_atcmd_back = "0, 476900000";
//                break;
            case 15:
                p_atcmd_type = "AT+PORT",p_atcmd_set = "?",p_atcmd_back = "0A";
                break;
            case 16:
//                p_atcmd_type = "AT+JOIN",p_atcmd_set = "?",p_atcmd_back = "0, 1, 1, 13, 176, 255, 0, 57600, 3600, 0, 0, 0";
                break;
            default:
                break;  
            }
        }
        break;
    case CN470P_JOIN_CMD://join atcmd
        {
            switch(atcmd_number)
            {
#if STD_CN470
          	case 0:
            p_atcmd_type = "AT+DEVEUI",p_atcmd_set = "=004A7700660014d0,D391010220102816,1",p_atcmd_back = "00 4A 77 00 66 00 14 D0";
          	break;
          	case 1:
            p_atcmd_type = "AT+APPEUI",p_atcmd_set = "=1122334455667788",p_atcmd_back = "11 22 33 44 55 66 77 88";
          	break;
          	case 2:
            p_atcmd_type = "AT+APPKEY",p_atcmd_set = "=00112233445566778899AABBCCDDEEFF,0",p_atcmd_back = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";
          	break;
          	case 3:
            p_atcmd_type = "AT+JOIN",p_atcmd_set = "=1",p_atcmd_back = "0";
          	break;
          	case 4:
            p_atcmd_type = "AT+BAND",p_atcmd_set = "=6",p_atcmd_back = "6";
          	break;
          	case 5:
            p_atcmd_type = "AT+CHMASK",p_atcmd_set = "=000000fe01",p_atcmd_back = "\0";
          	break;
            
#elif CN470P_SFREQ
            case 0:
            p_atcmd_type = "AT+DEVEUI",p_atcmd_set = "=004A7700660014d5,D391010220102816,1",p_atcmd_back = "00 4A 77 00 66 00 14 D5";
          	break;
          	case 1:
            p_atcmd_type = "AT+APPEUI",p_atcmd_set = "=1122334455667788",p_atcmd_back = "11 22 33 44 55 66 77 88";
          	break;
          	case 2:
            p_atcmd_type = "AT+APPKEY",p_atcmd_set = "=00112233445566778899AABBCCDDEEFF,0",p_atcmd_back = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";
          	break;
          	case 3:
            p_atcmd_type = "AT+JOIN",p_atcmd_set = "=1",p_atcmd_back = "0";
          	break;
          	case 4:
            p_atcmd_type = "AT+BAND",p_atcmd_set = "=7",p_atcmd_back = "7";
          	break;
          	case 5:
            p_atcmd_type = "AT+FREQ",p_atcmd_set = "=1,8,475300000",p_atcmd_back = "\0";
          	break;
            
#elif CN470P_DFREQ
            case 0:
            p_atcmd_type = "AT+DEVEUI",p_atcmd_set = "=004A7700660014d0,D391010220102816,1",p_atcmd_back = "00 4A 77 00 66 00 14 D5";
          	break;
          	case 1:
            p_atcmd_type = "AT+APPEUI",p_atcmd_set = "=1122334455667788",p_atcmd_back = "11 22 33 44 55 66 77 88";
          	break;
          	case 2:
            p_atcmd_type = "AT+APPKEY",p_atcmd_set = "=00112233445566778899AABBCCDDEEFF,0",p_atcmd_back = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";
          	break;
          	case 3:
            p_atcmd_type = "AT+JOIN",p_atcmd_set = "=1",p_atcmd_back = "0";
          	break;
          	case 4:
            p_atcmd_type = "AT+BAND",p_atcmd_set = "=6",p_atcmd_back = "6";
          	break;
          	case 5:
            p_atcmd_type = "AT+FREQ",p_atcmd_set = "=1,8,475300000",p_atcmd_back = "\0";
          	break;
            case 6:
            p_atcmd_type = "AT+FREQ",p_atcmd_set = "=3,8,505300000",p_atcmd_back = "\0";
          	break;
#endif
          	default:
          	break;
            }
        }
        break;
        
    case CLAA_JOIN_CMD://join atcmd
        { 
            switch(atcmd_number)
            {
            case 0:
                p_atcmd_type = "AT+DEVEUI",p_atcmd_set = "=004A7700660014d4,D391010220102816,1",p_atcmd_back = "00 4A 77 00 66 00 14 D4";
                break;
            case 1:
                p_atcmd_type = "AT+APPEUI",p_atcmd_set = "=2C26C50066000001",p_atcmd_back = "2C 26 C5 00 66 00 00 01";
                break;
            case 2:
                p_atcmd_type = "AT+APPKEY",p_atcmd_set = "=00112233445566778899AABBCCDDEEFF,0",p_atcmd_back = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";
                break;
            case 3:
                p_atcmd_type = "AT+JOIN",p_atcmd_set = "=1",p_atcmd_back = "0, 1, 1, 13, 1000, 255, 2, 57600, 3600, 1, 0";
                break;
            case 4:
                p_atcmd_type = "AT+BAND",p_atcmd_set = "=3,0",p_atcmd_back = "03 00";
                break;
            default:
                break;
            }  
        }
        break;
        
    case ICA_JOIN_CMD://join atcmd
        {
            switch(atcmd_number)
            {
            case 0:
                p_atcmd_type = "AT+DEVEUI",p_atcmd_set = "=d896e0ffff011956,D391010220102816,1",p_atcmd_back = "D8 96 E0 FF FF 01 19 56";
                break;
            case 1:
                p_atcmd_type = "AT+APPEUI",p_atcmd_set = "=d896e0e000002c0a",p_atcmd_back = "D8 96 E0 E0 00 00 2C 0A";
                break;
            case 2:
                p_atcmd_type = "AT+APPKEY",p_atcmd_set = "=31F6D88D801F2E5F1C6B79A231D98305,0",p_atcmd_back = "31 F6 D8 8D 80 1F 2E 5F 1C 6B 79 A2 31 D9 83 05";
                break;
            case 3:
                p_atcmd_type = "AT+JOIN",p_atcmd_set = "=1",p_atcmd_back = "0";
                break;
            case 4:
                p_atcmd_type = "AT+BAND",p_atcmd_set = "=47,0,0",p_atcmd_back = "47, 0, 0";
                break;
            case 6:
                p_atcmd_type = "AT+BAND",p_atcmd_set = "=47,0,1",p_atcmd_back = "47, 0, 1";
                break;
            case 7:
                p_atcmd_type = "AT+BAND",p_atcmd_set = "=47,4,1",p_atcmd_back = "47, 4, 1";
                break;
            case 8:
                p_atcmd_type = "AT+BAND",p_atcmd_set = "=47,4,0",p_atcmd_back = "47, 4, 0";
                break;
            default:
                break;
            }  
        }
        break;
        
    case COMMON_CMD://common atcmd
        {  
            switch(atcmd_number)
            {
            case 0:
                p_atcmd_type = "AT",p_atcmd_set = "\0",p_atcmd_back ="\0";
                break;  
//            case 1:
//                p_atcmd_type = "AT+FACTORY",p_atcmd_set = "\0",p_atcmd_back ="\0";
//                break;
            case 2:
                p_atcmd_type = "AT+GPIO",p_atcmd_set = "=255,1",p_atcmd_back ="\0";
                break;
            case 3:
                p_atcmd_type = "AT+DEBUG",p_atcmd_set = "=1",p_atcmd_back ="\0";
                break;
            case 4:
                p_atcmd_type = "AT+VER",p_atcmd_set = "?",p_atcmd_back ="\0";
                break;
            case 6:
                p_atcmd_type = "AT+CLASS",p_atcmd_set = "=0",p_atcmd_back = "0";
                break;
            case 7:
                p_atcmd_type = "AT+DATARATE",p_atcmd_set = "=3",p_atcmd_back = "3";
                break;
            case 8:
                p_atcmd_type = "AT+CONFIRM",p_atcmd_set = "=1,3",p_atcmd_back = "1, 3";
                break;
            case 9:
                p_atcmd_type = "AT+ADR",p_atcmd_set = "=1",p_atcmd_back = "1";
                break;
            case 10:
                p_atcmd_type = "AT+OTAA",p_atcmd_set = "=1",p_atcmd_back = "1";
                break;
            case 11:
                p_atcmd_type = "AT+POWER",p_atcmd_set = "=3",p_atcmd_back = "3";
                break;
            case 12:
                p_atcmd_type = "AT+RTC",p_atcmd_set = "=19,08,16,10,01,00",p_atcmd_back = "19 08 16 10 01";
                break;  
            default:
                break;  
            }
        }
        break;
        
    case SAVE_CMD:
        {
            switch(atcmd_number)
            {
            case 0:
                p_atcmd_type = "AT+SAVE",p_atcmd_set = "\0",p_atcmd_back ="\0";
                break;
            case 1:
                p_atcmd_type = "AT+RESET",p_atcmd_set = "\0",p_atcmd_back ="\0";
                break;
            default:
                break;
            }
        }
        break;
        
    default:
        break;
	}  
}

/*-----------------------------------------------------------------------------
����  ��Operate_Atcmd()
����  ��ATָ������
����  ��cmd��ָ�config_result����������
����ֵ��inquiry_res��У��ֵ  
RETURN_OK ������OK
RETURN_FALSE�����ش���
-----------------------------------------------------------------------------*/
uint8_t* Operate_Atcmd(uint8_t *cmd,const uint8_t *config_result,uint8_t *check)
{
	uint32_t i = 0;
	uint8_t atcmd_temp[ATCMD_MAX_LENGTH] = {0};
    
	PointertoArray(cmd,atcmd_temp);
	StringCat(atcmd_temp,config_result);//�ú����������м�"\r\n"
    debug_printf(atcmd_temp);
    
    LoRaNode_Wake_Sleep(MODE_WAKEUP);
	LoRaNode_Mode_Set(MODE_CMD);           /* set module in CMD Mode */
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
	LoRaNode_Send_Recv_AT(atcmd_temp,AT_Data_buf);
    
	//���ģ�鷵��"OK"
	if(StringStr(AT_Data_buf,"OK") != 0)
	{
        //����û����͵���"AT+FACTORY\r"���ָ��������ã�
        //����ģ���ʼ����ɺ�Ĭ��Ϊ͸��ģʽ��Ϊ�˽������û��ܲ�������ָ����Եȴ��ָ�����������ɺ�ʱ��1500ms
        if(StringStr(atcmd_temp,"AT+FACTORY") != 0)
        {
            LORANODE_DELAY(Factory_Init_Time);
        }
        
        //����û����͵���"AT+RESET\r"�������λ��
        //����ģ���ʼ����ɺ�Ĭ��Ϊ͸��ģʽ��Ϊ�˽������û��ܲ�������ָ����Եȴ���ʼ����ɺ�ʱ��150ms
        else if(StringStr(atcmd_temp,"AT+RESET") != 0)
        {
            LORANODE_DELAY(Reset_Init_Time);
            //���ģ����־��
            LoRaNode_Response_Recv(Timeout_Disa,No_Timeout,NULL);
        }
        
        memset(inquiry_res,0,ATCMD_MAX_LENGTH);
        
        if((*config_result == '?') && check[0] != '\0')     //��У��Ĳ�ѯ��ָ��
        {
            match_string(AT_Data_buf,":",RETURN_OK,inquiry_res);
            if(StringStr(inquiry_res,check))
                return inquiry_res;
            else return RETURN_FALSE;
        }
        else
        {
            while(AT_Data_buf[i])
            {
                inquiry_res[i] = AT_Data_buf[i];
                i++;
            }
            return inquiry_res;
        }
	}
	else  
	{
        return RETURN_FALSE;
	}
}

//ָ���Զ���Ⲣͳ��
void Atcmd_AutoDect(void)
{
	uint8_t at_number = 0;//ָ���������
	ATCMD_TYPE at_type_number = COMMON_CMD;//ָ�����ͼ���
	uint8_t atcmd_type[ATCMD_MAX_LENGTH] = {0};
//	uint8_t *atcmd_print = "\0";
	uint8_t *at_result = "\0";
	uint8_t *get_ver = "\0";
	uint8_t ver_flag = 0;//0 - ICA , 1 - CLAA
    
	get_ver = Operate_Atcmd("AT+VER","?","\0");
	if(StringStr(get_ver,"ICA"))
	{
		ver_flag = 1;
	}
	else if(StringStr(get_ver,"CLAA"))
	{
		ver_flag = 2;
	}
    
	for(at_type_number = FACTORY_CMD;at_type_number < NULL_CMD;at_type_number ++)
	{
        for(at_number = 0;at_number < 20;at_number ++)
        {
            if(ver_flag == 0)
			{
				if(at_type_number == CLAA_JOIN_CMD || at_type_number == ICA_JOIN_CMD)
					break;
			}
			else if(ver_flag == 1)
			{
				if(at_type_number == CN470P_JOIN_CMD || at_type_number == CLAA_JOIN_CMD)
					break;
			}
			else if(ver_flag == 2)
			{
				if(at_type_number == CN470P_JOIN_CMD || at_type_number == ICA_JOIN_CMD)
					break;
			}
            
            p_atcmd_type = "\0";
            p_atcmd_back = "\0";
            memset(atcmd_type,0,ATCMD_MAX_LENGTH);
            Atcmd_Combination(at_type_number,at_number); //��ȡatcmd_type��p_atcmd_set��p_atcmd_back��ֵ
            if(*p_atcmd_type == NULL)
            {
                continue;
            }
            // else continue;
            
            at_result = Operate_Atcmd(p_atcmd_type,p_atcmd_set,p_atcmd_back);
            PointertoArray(p_atcmd_type,atcmd_type);
            StringCat2(atcmd_type,p_atcmd_set);
            
            if(StringStr(at_result,RETURN_FALSE) == 0)  //��ѯ����������ȷ
            {
                at_success_count ++;                    //�ɹ�ָ��+1
            }
            else
            {
                at_failure_count ++;                   //ʧ��ָ��+1
                at_fail_id[at_failure_count-1] = at_id;
                atcmd_failure[at_failure_count-1] = p_atcmd_type;
            }
            Print_SingleATCmd_Result(atcmd_type,at_result);
            
            //If the instruction can be configured as a query instruction
            if((*p_atcmd_back) && (at_type_number != FACTORY_CMD))
            {
                memset(atcmd_type,0,ATCMD_MAX_LENGTH);
                p_atcmd_set = "?";
                at_result = Operate_Atcmd(p_atcmd_type,p_atcmd_set,p_atcmd_back);
                PointertoArray(p_atcmd_type,atcmd_type);
                StringCat(atcmd_type,p_atcmd_set);
                
                if(StringStr(at_result,RETURN_FALSE) == 0)  //��ѯ����������ȷ
                {
                    at_success_count ++;                    //�ɹ�ָ��+1
                }
                else
                {
                    at_failure_count ++;                   //ʧ��ָ��+1
                    at_fail_id[at_failure_count-1] = at_id;
                    atcmd_failure[at_failure_count-1] = p_atcmd_type;
                }
                Print_SingleATCmd_Result(atcmd_type,at_result);
            }  
        }
	}
# if 1
	SET_COMAT_DETECT("AT+DATARATE","=3",1);
	SET_COMAT_DETECT("AT+ADR","=1",20);
	SET_COMAT_DETECT("AT+POWER","=0",1);
    //SET_COMAT_DETECT("AT+BAUD","=2400",1);
	SET_COMAT_DETECT("AT+BAUD","=4800",1);
	SET_COMAT_DETECT("AT+BAUD","=115200",1);
	SET_COMAT_DETECT("AT+BAUD","=9600",1);
	LoRaNode_Join6(5,1);
	if(ver_flag == 2)
	{
        LoRaNode_Join6(5,0);
	}
# endif
	Send_AtCountDebug(at_success_count,at_failure_count);
}

//join detection
int8_t LoRaNode_JoinDetect(void)
{
    uint8_t debug_flag = 1;
    
    LoRaNode_Mode_Set(MODE_CMD);
    LoRaNode_GetJoinState(LoRa_Node_str);
	LoRaNode_Mode_Set(MODE_TRANSPARENT);
    if(LoRa_Node_str->Join == 0)
    {
        while(( LORANODE_BUSY_STATUS == GPIO_LOW_LEVEL ) || ( LORANODE_STAT_STATUS == GPIO_LOW_LEVEL ))//waiting for stat  
        {
            if(debug_flag)
            {
                debug_flag = 0;
                debug_printf("--> Please wait! The modem is joining\r\n");
            }
            if(Device_Sate == CMD_MODE)
            {
                break;
            }
            LoRaNode_Response_Recv(Timeout_Disa,No_Timeout,NULL);
            if(Time_Out_Break(Busy_PullHigh_Join_Max_Timeout) == 1)
            {
                debug_printf("The Module exception��It can't join the network!\r\n");
                return -1;
            }
        }
        TimeOut_Sign = 0;
        //Print the last log information of join
        while(LPUsart1_RX.receive_flag == 0)
        {
            if(Time_Out_Break(300) == 1)
            {
                break;
            }
        }
        TimeOut_Sign = 0;
        if(LPUsart1_RX.receive_flag == 1)
        {
            LPUsart1_RX.receive_flag = 0;
            PC_USART_DATA(LORANODE_USART_RECVIVE_BUF,LORANODE_USART_RECVIVE_LEN);
        }
        debug_printf("--> The modem is joined\r\n");
    }
    if(LoRa_Node_str->Join == 1)
    {
        debug_printf("--> The modem is joined\r\n");
        return 0;
    }
    return -1; 
    
}

//ͨ����ز�����
int8_t SET_COMAT_DETECT(uint8_t *p_at_com,const uint8_t *at_com_backval,uint8_t send_data_count)
{
	uint8_t *at_back = NULL,*adr_datarate_initval = "SF9",*power_initval = "TxPower: 20";
	uint8_t *send_concent = "112233";
	uint8_t sf_str[256] = {0};
	uint8_t send_fail_count = 0;
	uint8_t cmd_testok = 0;
	uint8_t at_com_arr[ATCMD_MAX_LENGTH] = {0};
	unsigned int baudval = 0,baudflag = 0;
//	PointertoArray(p_at_com,at_com_arr);
	at_back = Operate_Atcmd(p_at_com,at_com_backval,"\0");
	PointertoArray(p_at_com,at_com_arr);
	StringCat2(at_com_arr,at_com_backval);
    
	if(StringStr(at_back,RETURN_OK) == 0)//if cmd set failure,return
	{
        Print_SingleATCmd_Result(at_com_arr,at_back);
        
        at_failure_count ++;
        at_fail_id[at_failure_count-1] = at_id;
        atcmd_failure[at_failure_count-1] = p_at_com;
        return -1;
	}
    
    if(StringStr(p_at_com,"AT+BAUD"))
    {
        baudflag = 1;
        baudval = str2int(at_com_backval);
        lpuart1_reconfig(baudval);
        HAL_Delay(100);
    }
    
	if(LoRaNode_JoinDetect() == 0)//join successful
	{
       
        while(send_data_count)
        {
            send_log = LoRaNode_Write_Receive_Bytes(send_concent,strlen((const char*)send_concent),1);
//            LoRaNode_Response_Recv(Timeout_En,DataRecvive_Max_Timeout,NULL);
//            while(!Time_Out_Break(1000))
//            {
//                if(LPUsart1_RX.receive_flag == 1)
//                {
//                    LPUsart1_RX.receive_flag = 0;
//                    //    	USART2_SendString(LPUsart1_RX.RX_Buf);
//                    usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
//                }
//            }
//            TimeOut_Sign = 0;
            send_data_count --;
            if(send_log != 1)  
            {
                send_fail_count++;
            }
            if(send_fail_count >= 5)
            {
                usart2_send_string("Module exception or network problem!\r\n");
                send_fail_count = 0;
            }
        }
        send_fail_count = 0;
        if(baudflag)
        {
            baudflag = 0;
            baudval = 9600;
            LoRaNode_Mode_Set(MODE_CMD);  
            lpuart1_reconfig(baudval);
            HAL_Delay(100);
        }
        //Verify that the AT configuration takes effect
        if(!StringCmp(p_at_com,"AT+ADR"))
        {
            match_string(LPUsart1_RX.RX_Buf,"[UP]","[DN]",sf_str);
            if(StringStr(sf_str,adr_datarate_initval) == 0)//adr is ok
            {
                cmd_testok = 1;
            }
        }
        else if(!StringCmp(p_at_com,"AT+DATARATE"))
        {
            match_string(LPUsart1_RX.RX_Buf,"[UP]","[DN]",sf_str);
            if(StringStr(sf_str,adr_datarate_initval))
            {
                cmd_testok = 1;
            }
        }
        else if(!StringCmp(p_at_com,"AT+POWER"))
        {
            match_string(LPUsart1_RX.RX_Buf,"[UP]","[DN]",sf_str);
            if(StringStr(sf_str,power_initval))
            {
                cmd_testok = 1;
            }
        }
        else if(!StringCmp(p_at_com,"AT+BAUD"))
        {
            if(StringStr(LPUsart1_RX.RX_Buf,"[UP]"))
            {
                cmd_testok = 1;
            }
        }
        if(cmd_testok)
        {
            at_success_count ++;
            Print_SingleATCmd_Result(at_com_arr,at_back);
        }
        else
        {
            at_failure_count ++;
            at_fail_id[at_failure_count-1] = at_id;
            atcmd_failure[at_failure_count-1] = p_at_com;
            Print_SingleATCmd_Result(at_com_arr,"false");
        }
        cmd_testok = 0;
	}  
	return 0;
}

void Print_SingleATCmd_Result(uint8_t *send_atcmd,uint8_t* return_result)
{
	usart2_send_numbers(at_id);
	at_id ++;
	usart2_send_string("\r\n");
	usart2_send_string(send_atcmd);
	usart2_send_string(return_result);  
	usart2_send_string("\r\n");
}

void Send_AtCountDebug(uint8_t at_suc_cnt,uint8_t at_fail_cnt)
{
	usart2_send_string("Tested total: ");
	usart2_send_numbers(at_suc_cnt+at_fail_cnt);
	usart2_send_string("\r\nsuccess: ");
	usart2_send_numbers(at_suc_cnt);
	usart2_send_string(",failure: ");
	usart2_send_numbers(at_fail_cnt);
	usart2_send_string("\r\n");
	usart2_send_string("Fail ATCMD:\r\n");
	while(at_failure_count != 0)
	{
        usart2_send_numbers(at_fail_id[at_failure_count-1]);
        usart2_send_string(" ");
        usart2_send_string(atcmd_failure[at_failure_count-1]);
        usart2_send_string("\r\n");
        at_failure_count --;
	}
	at_success_count = 0;
	at_failure_count = 0;
}

//void LPurat1ToUart2_Debug(void)
//{
//	if(LPUsart1_RX.receive_flag == 1)
//	{
//        LPUsart1_RX.receive_flag = 0;
//        usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
//	}
//}
//ָ��ת�������麯��
void PointertoArray(uint8_t *p_str,uint8_t *array_str)
{
	uint8_t p_number = 0;
	if(*p_str == '\0')
        return;
	for(p_number = 0;p_number < strlen((const char*)p_str);p_number ++)
	{
        array_str[p_number] = p_str[p_number];
	}
}

int8_t LoRaNode_Join6(uint8_t send_data_cnt,uint8_t join6_success_fail)
{
	uint8_t Join6[20] = "AT+JOIN=6\r\n";
	uint8_t *temp = "OK";
	uint8_t *send_concent = "112233";
	uint8_t send_success_count = 0;
    
	LoRaNode_Mode_Set(MODE_CMD);
	if(join6_success_fail == 0)
	{
        if(LoRaNode_SetJoinRX1())
        {
            usart2_send_string("AT+RX1=1,9000,10000 setting is failure!\r\n");
            return -1;
        }
	}
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
	LoRaNode_Send_Recv_AT(Join6,AT_Data_buf);
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) == 0)
	{
        usart2_send_string("AT+JOIN=6 setting is failure!\r\n");
        return -1;
	}
    //�������
    LoRaNode_Mode_Set(MODE_TRANSPARENT);
    debug_printf("--> Please wait! The modem is joining\r\n");
    while(( LORANODE_BUSY_STATUS == GPIO_LOW_LEVEL ) || ( LORANODE_STAT_STATUS == GPIO_LOW_LEVEL ))//waiting for stat  
    {
        
        LoRaNode_Response_Recv(Timeout_Disa,No_Timeout,NULL);
        if(Time_Out_Break(Busy_PullHigh_Join_Max_Timeout) == 1)
        {
            debug_printf("The Module exception��It can't join the network!\r\n");
            return -1;
        }
    }
    TimeOut_Sign = 0;
    //Print the last log information of join
    while(LPUsart1_RX.receive_flag == 0)
    {
        if(Time_Out_Break(300) == 1)
        {
            break;
        }
    }
    TimeOut_Sign = 0;
    if(LPUsart1_RX.receive_flag == 1)
    {
        LPUsart1_RX.receive_flag = 0;
        PC_USART_DATA(LORANODE_USART_RECVIVE_BUF,LORANODE_USART_RECVIVE_LEN);
    }
    
	while(send_data_cnt)
	{
        send_log = LoRaNode_Write_Receive_Bytes(send_concent,strlen((const char*)send_concent),1);
        send_data_cnt --;
        if(send_log == 1)
        {
            send_success_count ++;
        }
	}
	if(send_success_count >= 3)
	{
        if(join6_success_fail)
        {
            at_success_count ++;
            Print_SingleATCmd_Result("AT+JOIN=6(1)","OK");
        }
        else
        {
            at_success_count ++;
            Print_SingleATCmd_Result("AT+JOIN=6(0)","OK");
        }
	}
	else
	{
        if(join6_success_fail)
        {
            at_failure_count ++;
            at_fail_id[at_failure_count-1] = at_id;
            atcmd_failure[at_failure_count-1] = "AT+JOIN=6(1)";
            Print_SingleATCmd_Result("AT+JOIN=6(1)","false");
        }  
        else
        {
            at_failure_count ++;
            at_fail_id[at_failure_count-1] = at_id;
            atcmd_failure[at_failure_count-1] = "AT+JOIN=6(0)";
            Print_SingleATCmd_Result("AT+JOIN=6(0)","false");
        }
        
	}
	if(join6_success_fail == 0)
	{
        LoRaNode_Mode_Set(MODE_CMD);
        if(LoRaNode_Reset())
        {
            usart2_send_string("AT+RESET setting is failure!\r\n");
            return -1;
        }
	}
	return 0;
}
/*-----------------------------------------------------------------------------
����  ��LoRaNode_Config_Single_AtCmd()
����  ������ATָ��
����  ��atcmd����Ҫ���õ�ATָ��
����ֵ����
-----------------------------------------------------------------------------*/
void LoRaNode_Config_Single_AtCmd(uint8_t* atcmd)
{  
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	//MCUͨ��lpuart1����ATָ���ģ�鲢��ȡģ��ķ���ֵ
	LoRaNode_Send_Recv_AT(atcmd,AT_Data_buf);
    
    //���ģ�鷵��"OK"
	if(StringStr(AT_Data_buf,"OK") != 0)
	{
        //����û����͵���"AT+FACTORY\r"���ָ��������ã�
        //����ģ���ʼ����ɺ�Ĭ��Ϊ͸��ģʽ��Ϊ�˽������û��ܲ�������ָ����Եȴ��ָ�����������ɺ�ʱ��1500ms
        if(StringStr(atcmd,"AT+FACTORY") != 0)
        {
            LORANODE_DELAY(Factory_Init_Time);
        }
        
        //����û����͵���"AT+RESET\r"�������λ��
        //����ģ���ʼ����ɺ�Ĭ��Ϊ͸��ģʽ��Ϊ�˽������û��ܲ�������ָ����Եȴ���ʼ����ɺ�ʱ��150ms
        else if(StringStr(atcmd,"AT+RESET") != 0)
        {
            LORANODE_DELAY(Reset_Init_Time);
        }
	}
}

/*-----------------------------------------------------------------------------
����  ��LoRaNode_Response_Recv()
����  ������ģ����Ӧ���ݣ�����ATָ����Ӧ����־�������������
����  ��timeout_enable���Ƿ�Ҫ��ʱ�����ı�־
        max_timeout����ʱʱ�䣬���ͣ�LoRaNode_Timing_t
        str:���յ���Ӧ��Ϣ����Ϊ��Ч�ַ��������ʾ���գ���ΪNULL�����ʾֱ��͸����PC���ڴ�ӡ����
����ֵ����
-----------------------------------------------------------------------------*/
void LoRaNode_Response_Recv(LoRaNode_Timeout_Enable timeout_enable,LoRaNode_Timing_t Max_Timeout,uint8_t* str)
{
	uint32_t num = 0;
	//�ȴ����ڽ����жϣ����������ճ�ʱ
    if(timeout_enable)
    {
        while(LORANODE_USART_RECVIVE_FLAG == 0)
        {
            if(TIMEOUT_COUNTER(Max_Timeout) == TIMEOUT)
            {
                UART_PRINT("The Time is over %d\r\n",Max_Timeout);              //�����ʱ����ӡ��ʱ����
                return;
            }
        }
        TimeOut_Sign = 0;

    }
    
	//������յ������ж�,��ȡ��������
	if(LORANODE_USART_RECVIVE_FLAG == 1)
	{
        LORANODE_USART_RECVIVE_FLAG = 0;                                        //��������ж�
        if(str != NULL)
        {
            for(num = 0 ; num < LORANODE_USART_RECVIVE_LEN; num++)              //��ȡģ�鷵������
            {
                str[num] = LORANODE_USART_RECVIVE_BUF[num];
            }
            PC_USART_DATA(LORANODE_USART_RECVIVE_BUF,LORANODE_USART_RECVIVE_LEN);//��ʱ�޸�
        }
        else
        {
            PC_USART_DATA(LORANODE_USART_RECVIVE_BUF,LORANODE_USART_RECVIVE_LEN);//��ӡ���ν��յ�����  
        }
    }
}





