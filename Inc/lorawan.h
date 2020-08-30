#ifndef __LORAWAN_H
#define __LORAWAN_H

#include "stm32l4xx_hal.h"


#define RXLEN	256

/*-------------------模块串口参数宏定义--------------------------------------*/
#define LORANODE_USART_RECVIVE_FLAG  LPUsart1_RX.receive_flag
#define LORANODE_USART_RECVIVE_BUF   LPUsart1_RX.RX_Buf
#define LORANODE_USART_RECVIVE_LEN   LPUsart1_RX.rx_len

/*状态信息输出宏定义*/
#define STATUS_DEBUG_ENABLE 0 

/*模块日志输出宏定义*/
#define LORANODE_DEBUG_ENABLE 0 

/*模块串口初始化宏定义*/
#define LORANODE_USART_INIT(x) LoRaNode_Usart_Init(x)

/*PC串口打印使能宏定义*/
#define PC_USART_PRINT_ENABLE 1

#if PC_USART_PRINT_ENABLE
#define  PC_DEBUG_INIT(x) PC_Usart_Init(x)
#define  PC_USART_STRING(x) usart2_send_string(x)
#define  PC_USART_DATA(x,y) usart2_send_data(x,y)
#define  UART_PRINT(fmt,arg...)  debug_printf(fmt,##arg) 

#else 
#define  PC_DEBUG_INIT(x)
#define  PC_USART_STRING(x)
#define  PC_USART_DATA(x,y)
#define  UART_PRINT(fmt,arg...)
#endif


//模块输出使能定义
typedef enum{
	Status_Debug_OFF,
	Status_Debug_ON,
}LoRaNode_Status_t;


//模块输出使能定义
typedef enum{
	NOT_TIMEOUT = 0,
	TIMEOUT     = 1,
}LoRaNode_Timeout_t;

typedef enum {
	MODE_INT,
	MODE_CMD,		//mode pin high
	MODE_TRANSPARENT,
} LoRaNode_Mode_T;

typedef enum {
	NET_ABP,
	NET_OTAA,
} LoRaNode_NetMode_T;

typedef enum {
	MODE_WAKEUP,
	MODE_SLEEP,	
} LoRaNode_SleepMode_T;

typedef struct {
	uint8_t DevEUI[8];	
	uint8_t AppEUI[8];	
	uint8_t DevADDR[4];	
	uint8_t AppKEY[16];	
	uint8_t AppSKEY[16];	
	uint8_t NwkSKEY[16];

	uint8_t ver;

	uint8_t Join;
	LoRaNode_Mode_T MODE;
	uint8_t ONline;
	uint8_t BUSY;
	uint8_t NET_Mode;
	uint8_t Confirm;
	uint8_t ADR;
	uint8_t SF;
	uint8_t NBT;
	uint8_t BAND;
	uint8_t POWER_code;
	uint8_t POWER;

	uint8_t P2P_Mode;
	uint8_t P2P_SF;
	uint16_t P2P_FRQ;
	uint8_t P2P_FRQ_step;
	uint8_t P2P_PWR;
	uint8_t P2P_PWR_code;
	uint8_t P2P_TIME;
	uint8_t P2P_RSSI[5];
	uint8_t P2P_SNR[5];
} Node_Info;

typedef struct {
	uint8_t Up_Result;	
	uint8_t Up_CH;	
	uint8_t Up_RATE;	
	uint8_t Up_DB;	
	uint32_t Up_Link;	
	uint16_t Up_Cache;
	uint16_t Resend;
} Send_Info;




/* Lorawan Node pin configuration */
#define LORANODE_WAKE_PIN					GPIO_PIN_3
#define LORANODE_WAKE_GPIO_PORT         	GPIOC
												
#define LORANODE_MODE_PIN					GPIO_PIN_0
#define LORANODE_MODE_GPIO_PORT         	GPIOA
													
#define LORANODE_NRST_PIN					GPIO_PIN_1
#define LORANODE_NRST_GPIO_PORT         	GPIOA
													
/* INPUT */													
#define LORANODE_STAT_PIN					GPIO_PIN_4
#define LORANODE_STAT_GPIO_PORT         	GPIOC
														
#define LORANODE_BUSY_PIN					GPIO_PIN_5
#define LORANODE_BUSY_GPIO_PORT         	GPIOC

/* stop the wfi sleep */		

#define LORANODE_WAKE_HIGH()    HAL_GPIO_WritePin(LORANODE_WAKE_GPIO_PORT, LORANODE_WAKE_PIN, GPIO_PIN_SET)
#define LORANODE_WAKE_LOW()     HAL_GPIO_WritePin(LORANODE_WAKE_GPIO_PORT, LORANODE_WAKE_PIN, GPIO_PIN_RESET)													
													
#define LORANODE_MODE_HIGH()    HAL_GPIO_WritePin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN, GPIO_PIN_SET)
#define LORANODE_MODE_LOW()     HAL_GPIO_WritePin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN, GPIO_PIN_RESET) 

#define LORANODE_NRST_HIGH()    HAL_GPIO_WritePin(LORANODE_NRST_GPIO_PORT, LORANODE_NRST_PIN, GPIO_PIN_SET)
#define LORANODE_NRST_LOW()     HAL_GPIO_WritePin(LORANODE_NRST_GPIO_PORT, LORANODE_NRST_PIN, GPIO_PIN_RESET)

#define LORANODE_STAT_STATUS	HAL_GPIO_ReadPin(LORANODE_STAT_GPIO_PORT, LORANODE_STAT_PIN)
#define LORANODE_BUSY_STATUS	HAL_GPIO_ReadPin(LORANODE_BUSY_GPIO_PORT, LORANODE_BUSY_PIN)
#define LORANODE_MODE_STATUS	HAL_GPIO_ReadPin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN)
#define LORANODE_WAKE_STATUS	HAL_GPIO_ReadPin(LORANODE_WAKE_GPIO_PORT, LORANODE_WAKE_PIN)

#define GPIO_HIGH_LEVEL         GPIO_PIN_SET
#define GPIO_LOW_LEVEL          GPIO_PIN_RESET

void LoRaNode_Init_Mode(LoRaNode_Mode_T mode);
void LoRaNode_Mode_Set(LoRaNode_Mode_T mode);
void LoRaNode_Wake_Sleep(LoRaNode_SleepMode_T mode);
uint8_t *LoRaNode_GetVer(void);
void LoRaNode_GetDevEUI(Node_Info *LoRa_temp);
void LoRaNode_GetAppEUI(Node_Info *LoRa_temp);
void LoRaNode_GetAppKey(Node_Info *LoRa_temp);
void LoRaNode_GetDevAddr(Node_Info *LoRa_temp);
void LoRaNode_GetAppSKey(Node_Info *LoRa_temp);
void LoRaNode_GetNwkSKey(Node_Info *LoRa_temp);
void LoRaNode_GetNetMode(Node_Info *LoRa_temp);
void LoRaNode_GetConfirm(Node_Info *LoRa_temp);
void LoRaNode_GetADR(Node_Info *LoRa_temp);
void LoRaNode_GetVER(Node_Info *LoRa_temp);
void LoRaNode_GetSF(Node_Info *LoRa_temp);
void LoRaNode_GetNBT(Node_Info *LoRa_temp);
void LoRaNode_GetBAND(Node_Info *LoRa_temp);
void LoRaNode_GetSNR(Node_Info *LoRa_temp);
void LoRaNode_GetPOWER(Node_Info *LoRa_temp);
void LoRaNode_GetState(Send_Info *LoRa_temp);
void LoRaNode_GetJoinState(Node_Info *LoRa_temp);

int8_t LoRaNode_SetGPIO(uint32_t pin, uint32_t state);
int8_t LoRaNode_SetNetMode(LoRaNode_NetMode_T mode);
int8_t LoRaNode_SetCFM(uint8_t nu);
int8_t LoRaNode_SetADR(uint8_t nu);
int8_t LoRaNode_SetSF(uint8_t nu);
int8_t LoRaNode_SetNBT(uint8_t nu);
int8_t LoRaNode_SetBAND(uint8_t nu);
int8_t LoRaNode_SetPOWER(uint8_t nu);
int8_t LoRaNode_SetDebug(uint32_t value);
//int8_t LoRaNode_SetRADIO(uint32_t f);
int8_t LoRaNode_SetP2P(uint32_t f,uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e,uint8_t ff,uint8_t g,uint16_t h);
int8_t LoRaNode_SetMINI_RF(uint32_t value);
int8_t LoRaNode_SetAppEUI(uint8_t *dev);
int8_t LoRaNode_SetAppKey(uint8_t *key);
int8_t LoRaNode_SetAppSKey(uint8_t *skey);
int8_t LoRaNode_SetNwkSKey(uint8_t *nwk_skey);
int8_t LoRaNode_Save(void);
int8_t LoRaNode_Reset(void);

void LoRaNode_OTAA_Config(void);
void LoRaNode_ABP_Config(void);

void GET_LoRaNode_Info(Node_Info *lora_temp);
void LoRaNode_STA_Check(Node_Info *LoRa_temp);

void LoRaNode_Send_Recv_AT(uint8_t *at_buf,uint8_t *str);
void LoRaNode_Send_AT(uint8_t *at_buf);
void LoRaNode_Read(uint8_t *str);

int8_t LoRaNode_LowPower_Send(uint8_t *buffer, int32_t len,uint8_t CONFIRM);
int8_t LoRaNode_Write_Receive_Bytes(uint8_t *buffer, int32_t len,uint8_t CONFIRM);

void Printf_LoRaNode_Info(Node_Info LoRa_temp);
int8_t LoRaNode_SetJoinRX1(void);
int8_t LoRaNode_SetFactory(void);


#endif 

