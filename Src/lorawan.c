#include <stdarg.h>
#include <string.h>
#include "lorawan.h"
#include "lowpower.h"
#include "app.h"
#include "str.h"
#include "usart.h"
#include "atcmd.h"
extern uint8_t TimeOut_Sign;
extern uint8_t *p_atcmd_set;
uint8_t AT_Data_buf[RXLEN];
Node_Info LoRa_Node;
Node_Info *LoRa_Node_str = &LoRa_Node;
Send_Info LoRa_Send;
Send_Info *LoRa_Send_str = &LoRa_Send;
extern uint8_t Send_Sate_Sign;
extern usart_recv_t Usart2_RX;
extern usart_recv_t Usart1_RX;
extern usart_recv_t LPUsart1_RX;
/* OTAA mode default setting */
//uint8_t devEui[] = "00 4A 77 00 66 FF FE CA";
uint8_t appEui[] = "11 22 33 44 55 66 77 88";
uint8_t appKey[] = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";
/* ABP mode default setting */
uint8_t appSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";
uint8_t appSKey_getpassword[] = "00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff , 0";
uint8_t nwkSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";
uint8_t devAddr[] = "FF FF FE A6";
uint8_t Key_show_flag = 0;                //???????,1????,0?????
//Pin WAKE/MODE/BUSY/RST config-----------------------------------------------------
//	@funtion:	initialization the lorawan module mode, command mode or transparent mode
//	@paramters:	MODE_CMD, or MODE_TRANSPARENT
//	@return:	void
//----------------------------------------------
void LoRaNode_Init_Mode(LoRaNode_Mode_T mode)
{
	LoRaNode_Reset();
    
    LORANODE_DELAY(150);
	LoRaNode_Wake_Sleep(MODE_WAKEUP);
    LORANODE_DELAY(50);
    
	LoRaNode_Mode_Set(mode);        
}
//---------------------------------------------
//	@funtion:	set the lorawan module mode, command mode or transparent mode
//	@paramters:	MODE_CMD, or MODE_TRANSPARENT
//	@return:	void
//----------------------------------------------
void LoRaNode_Mode_Set(LoRaNode_Mode_T mode)
{
	if (mode == MODE_CMD )
	{
        if(LORANODE_MODE_STATUS != GPIO_HIGH_LEVEL)
        {
            LORANODE_MODE_HIGH();
            LORANODE_DELAY(10);
        }
	}  
	else if (mode == MODE_TRANSPARENT)
	{
        if(LORANODE_MODE_STATUS != GPIO_LOW_LEVEL)
        {
            LORANODE_MODE_LOW();
            LORANODE_DELAY(10);
        }  
	}
     
}
//----------------------------------------------------------------------
//	@funtion:	set the lorawan module mode, wakeup or sleep mode
//	@paramters:	MODE_WAKEUP, or MODE_SLEEP
//	@return:	void
//-------------------------------------------------------------------
void LoRaNode_Wake_Sleep(LoRaNode_SleepMode_T mode)
{
	if (mode == MODE_WAKEUP)
	{
        /* wake signal, high for module wakeup, low for module sleep */  
        if (LORANODE_WAKE_STATUS != GPIO_HIGH_LEVEL)
        {
            /* wake module first, and wait 10 ms */
            LORANODE_WAKE_HIGH();
            LORANODE_DELAY(10);
        }
	}  
	else if (mode == MODE_SLEEP)
	{
        if (LORANODE_WAKE_STATUS != GPIO_LOW_LEVEL)
        {
            LORANODE_WAKE_LOW(); 
            LORANODE_DELAY(10);
        }
	}  
}
//Get AtCmd---------------------------------------------------------------------
//	@function:	get the lorawan module version
//	@return:	the point of version information
//
uint8_t *LoRaNode_GetVer(void)
{
	uint8_t ASK_VER[] = "AT+VER?\r\n";
	uint8_t *temp = "+VER:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_Recv_AT(ASK_VER,AT_Data_buf);
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        return ptr;  
	}
	return ptr;
}
/*
**	@function:	get deveui  
**	@return:	the point of the deveui
*/
uint8_t DEVEUI[17] = {0};
void LoRaNode_GetDevEUI(Node_Info *LoRa_temp)
{
	uint8_t GetDevEUI[] = "AT+DEVEUI?\r\n";
	uint8_t *temp = "+DEVEUI:";
	uint8_t *ptr = NULL;
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetDevEUI);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */  
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<8;i++)
        {
            DEVEUI[2*i] = *((ptr + 8+(3*i))+1);
            DEVEUI[2*i+1] = *((ptr + 8+(3*i))+2);
        }
        for(uint8_t i=0;i<8;i++)
        {
            temp1 = *((ptr + 8+(3*i))+1);
            temp2 = *((ptr + 8+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->DevEUI[i] = temp1*16 + temp2;
        }
	}
}
/*
**	@function:	get appeui  
**	@return:	the point of the appeui
*/
void LoRaNode_GetAppEUI(Node_Info *LoRa_temp)
{
	uint8_t GetAPPEUI[] = "AT+APPEUI?\r\n";
	uint8_t *temp = "+APPEUI:";
	uint8_t *ptr = NULL;
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetAPPEUI);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<8;i++)
        {
            temp1 = *((ptr + 8+(3*i))+1);
            temp2 = *((ptr + 8+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->AppEUI[i] = temp1*16 + temp2;
        }
	}
    
}
/*
**	@function:	get appkey  
**	@return:	the point of the response of appkey
*/
void LoRaNode_GetAppKey(Node_Info *LoRa_temp)
{
	uint8_t GetAPPKEY[] = "AT+APPKEY?\r\n";  
	uint8_t *temp = "+APPKEY:";
	uint8_t *ptr = NULL;  
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetAPPKEY);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<16;i++)
        {
            temp1 = *((ptr + 8+(3*i))+1);
            temp2 = *((ptr + 8+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->AppKEY[i] = temp1*16 + temp2;
        }
	}
    
}
/*
**	@function:	get devaddr  
**	@return:	the point of the Devaddr
*/
void LoRaNode_GetDevAddr(Node_Info *LoRa_temp)
{
	uint8_t GetDEVAddr[] = "AT+DEVADDR?\r\n";
	uint8_t *temp = "+DEVADDR:";
	uint8_t *ptr = NULL;
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetDEVAddr);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<4;i++)
        {
            temp1 = *((ptr + 9+(3*i))+1);
            temp2 = *((ptr + 9+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->DevADDR[i] = temp1*16 + temp2;
        }
	}
    
}
/*
**	@function:	get appskey  
**	@return:	the point of the response of appskey
*/
void LoRaNode_GetAppSKey(Node_Info *LoRa_temp)
{
	uint8_t GetAppSKey[] = "AT+APPSKEY?\r\n";
	uint8_t *temp = "+APPSKEY:";
	uint8_t *ptr = NULL;
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetAppSKey);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<16;i++)
        {
            temp1 = *((ptr + 9+(3*i))+1);
            temp2 = *((ptr + 9+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->AppSKEY[i] = temp1*16 + temp2;
        }
	}
}
/*
**	@function:	get nwkskey  
**	@return:	the point of the response of nwkskey
*/
void LoRaNode_GetNwkSKey(Node_Info *LoRa_temp)
{
	uint8_t GetNwkSKey[] = "AT+NWKSKEY?\r\n";  
	uint8_t *temp = "+NWKSKEY:";
	uint8_t *ptr = NULL;  
	uint8_t temp1=0,temp2=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetNwkSKey);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i=0;i<16;i++)
        {
            temp1 = *((ptr + 9+(3*i))+1);
            temp2 = *((ptr + 9+(3*i))+2);
            if(temp1 > 0x40)
            {
                temp1 = temp1 - 55;
            }else{
                temp1 = temp1 - 48;
                
            }
            if(temp2 > 0x40)
            {
                temp2 = temp2 - 55;
            }else{
                temp2 = temp2 - 48;
            }
            LoRa_temp->NwkSKEY[i] = temp1*16 + temp2;
        }
	}
}
/*
**	@function:	get uplink value
**	@return:	the point of the uplink buffer
*/
void LoRaNode_GetNetMode(Node_Info *LoRa_temp)
{
	uint8_t GetNetMode[] = "AT+OTAA?\r\n";
	uint8_t *temp = "+OTAA:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetNetMode);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        if( *(ptr + 7) == 0x30)
        {
            LoRa_temp->NET_Mode = 0;
        }
        if(*(ptr + 7) == 0x31)
        {
            LoRa_temp->NET_Mode = 1;
        }
	}
    
}
/*
**	@function:	get uplink value
**	@return:	the point of the uplink buffer
*/
void LoRaNode_GetConfirm(Node_Info *LoRa_temp)
{
	uint8_t GetConfirm[] = "AT+CONFIRM?\r\n";
	uint8_t *temp = "+CONFIRM:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetConfirm);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        if(*(ptr + 10) == 0x30)
        {
            LoRa_temp->Confirm= 0;
        }
        if(*(ptr + 10) == 0x31)
        {
            LoRa_temp->Confirm= 1;
        }
	}
}
void LoRaNode_GetADR(Node_Info *LoRa_temp)
{
	uint8_t ADR[] = "AT+ADR?\r\n";
	uint8_t *temp = "+ADR:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(ADR);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        if(*(ptr + 6) == 0x30)
        {
            LoRa_temp->ADR= 0;
        }
        if(*(ptr + 6) == 0x31)
        {
            LoRa_temp->ADR= 1;
        }
	}
    
}
void LoRaNode_GetVER(Node_Info *LoRa_temp)
{
	uint8_t VER[] = "AT+VER?\r\n";
	uint8_t *temp = "CN470";
	uint8_t *temp1 = "CLAA";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(VER);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        LoRa_temp->ver = 2; // cn470
	}
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp1)) != NULL)
	{
        LoRa_temp->ver = 1; // claa
	}
}
void LoRaNode_GetSF(Node_Info *LoRa_temp)
{
	uint8_t SF[] = "AT+DATARATE?\r\n";
	uint8_t *temp = "+DATARATE:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SF);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        LoRa_temp->SF = (*(ptr + 11)) - 0x30;  
	}
    
}
void LoRaNode_GetNBT(Node_Info *LoRa_temp)
{
	uint8_t GetNBTRIALS[] = "AT+NBTRIALS?\r\n";
	uint8_t *temp = "+NBTRIALS:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetNBTRIALS);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        LoRa_temp->NBT = (*(ptr + 11)) - 0x30;  
	}
    
}
void LoRaNode_GetBAND(Node_Info *LoRa_temp)
{
	uint8_t GetBAND[] = "AT+BAND?\r\n";
	uint8_t *temp = "+BAND:";
	uint8_t *ptr = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetBAND);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        LoRa_temp->BAND = (*(ptr + 7)) - 0x30;  
	}
    
}
void LoRaNode_GetSNR(Node_Info *LoRa_temp)
{
	uint8_t GetSNR[] = "AT+SNR?\r\n";
	uint8_t *temp = ",";
	uint8_t *temp1 = "]";
	uint8_t *ptr = NULL;
	uint8_t *ptr1 = NULL;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetSNR);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr1 = StringStr((uint8_t *)AT_Data_buf, temp1)) != NULL)
	{
        for(uint8_t i = 0;i<5;i++)
        {
            if(*(ptr1+i+3) == 0x2c)
            {
                break;
            }else
            {
                LoRa_temp->P2P_RSSI[i] = *(ptr1+i+3);
            }
        }
	}
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        for(uint8_t i = 0;i<5;i++)
        {
            if(*(ptr+i+2) == 0x0d)
            {
                break;
            }else
            {
                LoRa_temp->P2P_SNR[i] = *(ptr+i+2);
            }
        }
        
	}
    
}
void LoRaNode_GetPOWER(Node_Info *LoRa_temp)
{
	uint8_t GetPOWER[] = "AT+POWER?\r\n";
	uint8_t *temp = "+POWER:";
	uint8_t *ptr = NULL;
	uint8_t code = 0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetPOWER);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        LoRa_temp->POWER_code = (*(ptr + 8)) - 0x30;  
        code = LoRa_temp->POWER_code;
	}
	if(code == 0)
	{
        LoRa_temp->POWER = 20;
	}
	if(code == 1)
	{
        LoRa_temp->POWER = 17;
	}
	if(code == 2)
	{
        LoRa_temp->POWER = 16;
	}
	if(code == 3)
	{
        LoRa_temp->POWER = 14;
	}
	if(code == 4)
	{
        LoRa_temp->POWER = 12;
	}
	if(code == 5)
	{
        LoRa_temp->POWER = 10;
	}
	if(code == 6)
	{
        LoRa_temp->POWER = 7;
	}
	if(code == 7)
	{
        LoRa_temp->POWER = 5;
	}
	if(code == 8)
	{
        LoRa_temp->POWER = 2;
	}
    
}
void LoRaNode_GetState(Send_Info *LoRa_temp)
{
	uint8_t GetStatus[] = "AT+STATUS?\r\n";
	uint8_t *temp = "+STATUS:";
	uint8_t *ptr = NULL;
	uint8_t dec=0,dec1=0,dec2=0,dec3=0,dec4=0,dec5=0,dec6=0,dec7=0;
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GetStatus);
	HAL_Delay(200);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        dec =StrToHex( *(ptr + 9));
        dec1 = StrToHex( *(ptr + 10));
        LoRa_temp->Up_Result = dec*16 + dec1;
        dec = StrToHex( *(ptr + 12));
        dec1 = StrToHex( *(ptr + 13));
        LoRa_temp->Up_CH = dec*16 + dec1;
        dec = StrToHex( *(ptr + 15));
        dec1 = StrToHex( *(ptr + 16));
        LoRa_temp->Up_RATE = dec*16 + dec1;
        dec = StrToHex( *(ptr + 18));
        dec1 = StrToHex( *(ptr + 19));
        LoRa_temp->Up_DB = dec*16 + dec1;
        dec = StrToHex( *(ptr + 66));
        dec1 = StrToHex( *(ptr + 67));
        dec2 = StrToHex( *(ptr + 69));
        dec3 = StrToHex( *(ptr + 70));
        LoRa_temp->Up_Cache = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;
        dec = StrToHex( *(ptr + 72));
        dec1 = StrToHex( *(ptr + 73));
        dec2 = StrToHex( *(ptr + 75));
        dec3 = StrToHex( *(ptr + 76));
        LoRa_temp->Resend = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;
        dec = StrToHex( *(ptr + 117));
        dec1 = StrToHex( *(ptr + 118));
        dec2 = StrToHex( *(ptr + 120));
        dec3 = StrToHex( *(ptr + 121));
        dec4 = StrToHex( *(ptr + 123));
        dec5 = StrToHex( *(ptr + 124));
        dec6 = StrToHex( *(ptr + 126));
        dec7 = StrToHex( *(ptr + 127));
        
        LoRa_temp->Up_Link = (dec*16+ dec1) +(dec2*16 + dec3)*256 + (dec4*16+ dec5)*65536 +(dec6*16 + dec7)*256*65536 ;  
	}
}
void LoRaNode_GetJoinState(Node_Info *LoRa_temp)
{
	uint8_t GetJoin[] = "AT+JOIN?\r\n";
	uint8_t *temp = "+JOIN:";
	uint8_t *ptr = NULL;
	uint8_t JoinTmp = 0,JoinConnStat = 0;
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_Recv_AT(GetJoin,AT_Data_buf);
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        JoinTmp = StrToHex( *(ptr + 7));
        if(JoinTmp)
            LoRa_Node_str->Join = 1;
        else  
            LoRa_Node_str->Join = 0;
        //  JoinConnStat = StrToHex( *(ptr + 49)); //unconnected or connected or Lost connection
        //  if((JoinTmp == 0) && (JoinConnStat == 0))
        //  {}
        //  else if((JoinTmp == 0) && (JoinConnStat))    
	}
}
//Set ATCmd---------------------------------------------------------------------
/*
**	@function:	set gpio state
**	@paramters:	pin, 0, 1, 2, 3, 255; state 0, 1
**	@return:	0 success, 1 bad paramter
*/
int8_t LoRaNode_SetGPIO(uint32_t pin, uint32_t state)
{  
	uint8_t GPIO[20] = "AT+GPIO=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
	uint8_t *ptr = (uint8_t*)GPIO;
    
	Int2Str(buf, pin);
	StringCat((uint8_t *)GPIO, (uint8_t *)buf);
    
	while(*++ptr);	//get the end
	*ptr++ = ',';
	memset(buf,0,5);
	Int2Str(buf, state);
	StringCat(GPIO, buf);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(GPIO);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if((ptr = StringStr((uint8_t *)AT_Data_buf, temp)) != NULL)
	{
        return 0;
	}  
	return -1;
}
/*
**	@function:	set net mode
**	@paramters: 0, abp mode; 1, otaa mode
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetNetMode(LoRaNode_NetMode_T mode)
{  
	uint8_t NetMode[20] = "AT+OTAA=";
	uint8_t buf[10] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, mode);
	StringCat(NetMode, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(NetMode);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetCFM(uint8_t nu)
{  
	uint8_t CFM[20] = "AT+CONFIRM=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(CFM, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(CFM);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetADR(uint8_t nu)
{  
	uint8_t ADR[20] = "AT+ADR=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(ADR, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(ADR);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetSF(uint8_t nu)
{  
	uint8_t SF[20] = "AT+DATARATE=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(SF, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SF);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetNBT(uint8_t nu)
{  
	uint8_t NBT[20] = "AT+NBTRIALS=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(NBT, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(NBT);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetBAND(uint8_t nu)
{  
	uint8_t BAND[20] = "AT+band=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(BAND, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(BAND);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_SetPOWER(uint8_t nu)
{  
	uint8_t POWER[20] = "AT+POWER=";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, nu);
	StringCat(POWER, buf);
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(POWER);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	set debug value
**	@paramters: 0, release; 1, debug
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetDebug(uint32_t value)
{
	uint8_t SetDebug[20] = "AT+DEBUG=";
	uint8_t buf[10] = {0};
	uint8_t *temp = "OK";
    
	Int2Str(buf, value);
	StringCat(SetDebug, buf);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetDebug);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
	return -1;
}
int8_t LoRaNode_SetP2P(uint32_t f,uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e,uint8_t ff,uint8_t g,uint16_t h)
{
	uint8_t SetRADIO[50] = "AT+RADIO=";
	uint8_t buf[10] = {0};  
	uint8_t buf1[10] = {0};  
	uint8_t buf2[10] = {0};  
	uint8_t buf3[10] = {0};  
	uint8_t buf4[10] = {0};  
	uint8_t buf5[10] = {0};  
	uint8_t buf6[10] = {0};  
	uint8_t buf7[10] = {0};  
	uint8_t buf8[10] = {0};  
    
	uint8_t dou[2] = ",";
	uint8_t *temp = "OK";
    
	Int2Str(buf, f);
	StringCat2(SetRADIO, buf);
	StringCat2(SetRADIO, dou);
	Int2Str(buf1, a);
	StringCat2(SetRADIO, buf1);
	StringCat2(SetRADIO, dou);
	Int2Str(buf2, b);
	StringCat2(SetRADIO, buf2);
	StringCat2(SetRADIO, dou);
	Int2Str(buf3, c);
	StringCat2(SetRADIO, buf3);
	StringCat2(SetRADIO, dou);
	Int2Str(buf4, d);
	StringCat2(SetRADIO, buf4);
	StringCat2(SetRADIO, dou);
	Int2Str(buf5, e);
	StringCat2(SetRADIO, buf5);
	StringCat2(SetRADIO, dou);
	Int2Str(buf6, ff);
	StringCat2(SetRADIO, buf6);
	StringCat2(SetRADIO, dou);
	Int2Str(buf7, g);
	StringCat2(SetRADIO, buf7);
	StringCat2(SetRADIO, dou);
	Int2Str(buf8, h);
	StringCat(SetRADIO, buf8);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetRADIO);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	set debug value
**	@paramters: 0, release; 1, debug
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetMINI_RF(uint32_t value)
{
	uint8_t MINI[20] = "AT+MINIRF=";
	uint8_t buf[10] = {0};  
	uint8_t *temp = "OK";
    
	Int2Str(buf, value);
	StringCat(MINI, buf);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(MINI);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	set appeui  
**	@paramters: appeui 8 bytes
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetAppEUI(uint8_t *dev)
{
	uint8_t SetAppEUI[50] = "AT+APPEUI=";
	uint8_t *temp = "OK";
    
	StringCat(SetAppEUI, dev);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetAppEUI);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
	return -1;
}
/*
**	@function:	set appkey, only in otaa mode
**	@paramters: appkey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetAppKey(uint8_t *key)
{
	uint8_t SetAppKey[80] = "AT+APPKEY=";  
	uint8_t *temp = "OK";
    
	StringCat(SetAppKey, key);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetAppKey);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	set appskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetAppSKey(uint8_t *skey)
{
	uint8_t SetAppSKey[80] = "AT+APPSKEY=";
	uint8_t *temp = "OK";
    
	StringCat(SetAppSKey, skey);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetAppSKey);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	set nwkskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_SetNwkSKey(uint8_t *nwk_skey)
{
	uint8_t SetNwkSKey[80] = "AT+NWKSKEY=";
	uint8_t *temp = "OK";
    
	StringCat(SetNwkSKey, nwk_skey);
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SetNwkSKey);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
    
	return -1;
}
/*
**	@function:	save config
**	@return:	0 success, -1 bad paramter
*/
int8_t LoRaNode_Save(void)
{
	uint8_t SAVE[] = "AT+SAVE\n\r";
	uint8_t *temp = "OK";
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(SAVE);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{  
        return 0;
	}
    
	return -1;
}
int8_t LoRaNode_Reset(void)
{
	uint8_t RESET[] = "AT+Reset\n\r";
	uint8_t *temp = "OK";
    
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_AT(RESET);
    
	LoRaNode_Read(AT_Data_buf);    /* receive the response of this command */
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{  
        return 0;
	}
    
	return -1;
}
void LoRaNode_Send_Recv_AT(uint8_t *at_buf,uint8_t *str)
{
	LPUsart1_RX.receive_flag = 0; //Clear the serial port interrupt flag to prevent accidents
	lpusart1_send_string(at_buf);
	LoRaNode_Response_Recv(Timeout_En,ATCmd_Response_Max_Timeout,str);
}

void LoRaNode_Send_AT(uint8_t *at_buf)
{
	lpusart1_send_string(at_buf);
	HAL_Delay(200);
}
void LoRaNode_Read(uint8_t *str)
{
	uint32_t i = 0;
	if(LPUsart1_RX.receive_flag == 1)
	{
        LPUsart1_RX.receive_flag = 0;
        for(i=0 ; i < LPUsart1_RX.rx_len; i++)
        {
            str[i] = LPUsart1_RX.RX_Buf[i];
        }
	}
}
//PtoP-------------------------------------------------------------------------------------------
int8_t LoRaNode_LowPower_Send(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	int8_t ret = 0;
    
	//----???? ----
	LoRaNode_Wake_Sleep(MODE_WAKEUP);
	LORANODE_DELAY(10);
	//----??????????
	if(LORANODE_MODE_STATUS == 0)  
	{
        //----?? BUSY ????????(???),??????????
        while(LORANODE_BUSY_STATUS == GPIO_PIN_RESET)
        {
            if(Time_Out_Break(2000) == 1)
            {
                ret = -1;             // ?? -1  : ???????
                return ret;
            }
        }
        TimeOut_Sign = 0;
        //----????1?????????
        lpusart1_send_data(buffer, len);
        //----?? BUSY ????????(???),??????????
        while(LORANODE_BUSY_STATUS == GPIO_PIN_SET)
        {
            if(Time_Out_Break(2000) == 1)
            {
                ret = -2;             // ?? -2  : ??????????????,????????
                return ret;
            }
        }
        TimeOut_Sign = 0;
        //-----?????????----------
        Send_Sate_Sign = 1;          //??????????
        Set_RTC_WakeUp(30);
        LoRaNode_Send_To_Sleep();
        if(Send_Sate_Sign == 11)
        {
            Send_Sate_Sign = 0;
            ret = -3;             // ?? -2  : ??????????
            return ret;
        }
        if(Send_Sate_Sign == 12)
        {
            Send_Sate_Sign = 0;
            ret = 2;             // ??????????????
            return ret;
        }
        if(Send_Sate_Sign == 13)
        {
            Send_Sate_Sign = 0;
            if(CONFIRM == 0)
            {
                ret = 1;             // ????
                return ret;
            }else
            {
                while(LORANODE_STAT_STATUS == GPIO_PIN_RESET)
                {
                    if(Time_Out_Break(1000) == 1)
                    {
                        ret = -4;             // ?? -4  : ??????????????,????????
                        return ret;
                    }
                }
                TimeOut_Sign = 0;
                ret = 1;            // ????
                return ret;
            }
        }  
	}  
}
/*--------------------LoRaNode_Write_Receive_Bytes------------------------------
*return value "ret":
*   1--> data transmit success
*   2--> module is busy  
*   3--> module transmit timeout  
*   4--> module did not receive ack
*/
int8_t LoRaNode_Write_Receive_Bytes(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	int8_t ret = 0;
	//----wakeup module ----
	LoRaNode_Wake_Sleep(MODE_WAKEUP);
    LoRaNode_Mode_Set(MODE_TRANSPARENT);
	//----Determine if the module is in transparent mode
    //----waiting BUSY is high level
    while(LORANODE_BUSY_STATUS == GPIO_PIN_RESET)
    {
        
//            if(LPUsart1_RX.receive_flag == 1)
//            {
//                LPUsart1_RX.receive_flag = 0;
//                usart2_send_data(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
//            }
//            LoRaNode_Response_Recv(Timeout_Disa,No_Timeout,NULL);
        
        if(Time_Out_Break(2000) == 1)
        {
            ret = -1;             // error -1  : module is busy
            TimeOut_Sign = 0;
            return ret;
        }
        
    }
    TimeOut_Sign = 0;
    //----LPusart1 send the user's data
    lpusart1_send_data(buffer, len);
    //----waiting BUSY is low level
    while(LORANODE_BUSY_STATUS == GPIO_PIN_SET)
    {
        if(Time_Out_Break(2000) == 1)
        {
            ret = -2;             // error -2: module did not receive the data sent by the serial port
            return ret;
        }
    }
    TimeOut_Sign = 0;
    //----waiting BUSY is high level
    do
    {
        while(LORANODE_BUSY_STATUS == GPIO_PIN_RESET)
        {
            //Do not receive new user data when the module is sending data
            if(Usart2_RX.receive_flag)
            {
                Usart2_RX.receive_flag = 0;
                debug_printf("--> Warning: Don't send data now! The modem is busy!\r\n");
            }
            if(Time_Out_Break(60000) == 1)
            {
                ret = -3;             // error -3: Module send timeout
                return ret;
            }
        }
        TimeOut_Sign = 0;
        
        while(LORANODE_BUSY_STATUS == GPIO_PIN_SET)
		{
			if(1 == Time_Out_Break(100))
			{
				break;
			}
		}
        TimeOut_Sign = 0;
        //receive download data or ack
        LoRaNode_Response_Recv(Timeout_En,DataRecvive_Max_Timeout,NULL);    
    }
    while(LORANODE_BUSY_STATUS == GPIO_PIN_RESET); 
    //Determine the STAT pin level status
    HAL_Delay(1);
    if(LORANODE_STAT_STATUS == GPIO_PIN_SET)
    {
        ret = 1;             // correct 1 : Data communication success!
    } 
    if(ret == 1)
    {
        return ret;
    }
    else
    {
        ret = -4;                    // error -4 : Data communication failure!
        return ret;
    }
}


void LoRaNode_STA_Check(Node_Info *LoRa_temp)
{
	static uint8_t online_log = 0;
	if(LORANODE_STAT_STATUS == 0)
	{
        LoRa_temp->ONline = 0;
	}else
    {
        LoRa_temp->ONline = 1;
    }
	if(LORANODE_BUSY_STATUS == 0)
	{
        LoRa_temp->BUSY = 0;
	}else
    {
        LoRa_temp->BUSY = 1;
    }
	if(LoRa_temp->NET_Mode == 1)
	{
        if(LoRa_temp->ONline == 1)
        {
            if(LoRa_temp->BUSY == 1)
            {
                LoRa_temp->Join = 1;
            }
        }else{
            LoRa_temp->Join = 0;
        }
	}else if(LoRa_temp->NET_Mode == 0)
    {
        if(LoRa_temp->BUSY == 1)
        {
            LoRa_temp->Join = 1;
        }else{
            LoRa_temp->Join = 0;
        }
    }
	if(LoRa_temp->Join != online_log )
	{
        if(LoRa_temp->Join == 1)
        {
            debug_printf("--> The network is joined\r\n");
        }else
        {
            //  	Uart_Printf("\r\n--> \r\n");
        }
	}  
	online_log = LoRa_temp->Join;
}

void LoRaNode_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}

int8_t LoRaNode_SetJoinRX1(void)
{  
	uint8_t Join6[25] = "AT+RX1=1,9000,10000\r\n";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_Recv_AT(Join6,AT_Data_buf);
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
	return -1;
}

int8_t LoRaNode_SetFactory(void)
{  
	uint8_t fact[25] = "AT+FACTORY\r\n";
	uint8_t buf[5] = {0};
	uint8_t *temp = "OK";
	memset(AT_Data_buf,0,RXLEN);        /* clear the buffer and wPoint */
    
	LoRaNode_Send_Recv_AT(fact,AT_Data_buf);
    
	if(StringStr((uint8_t *)AT_Data_buf, temp) != NULL)
	{
        return 0;
	}
	return -1;
}


