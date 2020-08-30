#ifndef __ATCMD_H__
#define __ATCMD_H__
#include "stdint.h"



#define ATCMD_MAX_LENGTH 256
#define ATCMD_MAX_FAILCOUNT 128
#define RETURN_OK "OK"
#define RETURN_FALSE "false"


#define STD_CN470        0    //标准470

#define CN470P_SFREQ     0    //自主470同频

#define CN470P_DFREQ     1    //自主470异频

/*--------------------------系统延时时间宏定义--------------------------------*/
#define LORANODE_DELAY(x) HAL_Delay(x)

#define TIMEOUT_COUNTER(x) Time_Out_Break(x)

/*RTC定时唤醒系统时间，单位s*/
#define RTC_Wakeup_Time  30

/*模块各功能时间枚举*/
typedef enum       //单位：ms
{
	PowerOn_Init_Time = 500,                    //上电初始化时间
	Wake_PullLow_Time = 10,                     //wake拉低时间
	Wake_PullHigh_Time = 10,                    //wake拉高时间
	ATCmd_Response_Max_Timeout = 2000,          //指令响应最大超时时间
	Commond_Mode_Stime = 10,                    //切换至指令模式时间
	Transparent_Mode_Stime = 30,                //切换至透传模式时间
	Factory_Init_Time = 1500,                   //恢复出厂设置后初始化时间
	Reset_Init_Time = 150,                      //复位后初始化时间
	Busy_PullLow_CmdToTrans_Timeout = 1000,     //指令切透传等待busy拉低超时时间
	Busy_PullHigh_Join_Max_Timeout = 240000,    //入网等待busy拉高最大超时时间
	Busy_Wakeup_PullHigh_Timeout = 1000,        //唤醒后等待busy拉高超时时间
	Busy_SendDataCmt_PullLow_Timeout = 1000,    //串口数据发送完成后，等待busy拉低超时时间
	Busy_DataCommunicateCmt_PullHigh = 60000,   //数据通信完成后，等待busy拉高超时时间
	DataRecvive_Max_Timeout = 2000,              //数据通信完成后，接收下行数据最大等待时间,适用于各个波特率
	Debug_JoinSuccess_Timeout = 300,            //如果debug打开，入网成功后最后一包日志打印超时时间
	Reset_Init_Timeout = 300,                   //如果debug打开，reset之后打印初始化日志超时时间
    No_Timeout = 0,
}LoRaNode_Timing_t;

typedef enum
{
    Timeout_Disa = 0,
    Timeout_En,
}LoRaNode_Timeout_Enable;

typedef enum
{
    FACTORY_CMD = 0,
	CN470P_JOIN_CMD,
	CLAA_JOIN_CMD,
	ICA_JOIN_CMD,
    COMMON_CMD,
	SAVE_CMD,
	NULL_CMD,
}ATCMD_TYPE;





uint8_t* Operate_Atcmd(uint8_t *cmd,const uint8_t *config_result,uint8_t *check);
void Print_SingleATCmd_Result(uint8_t *send_atcmd,uint8_t* return_result);
void Atcmd_Combination(ATCMD_TYPE at_type, uint8_t atcmd_number);
void PointertoArray(uint8_t *p_str,uint8_t *array_str);
void Atcmd_AutoDect(void);
void Send_AtCountDebug(uint8_t at_suc_cnt,uint8_t at_fail_cnt);
void LPurat1ToUart2_Debug(void);
int8_t LoRaNode_JoinDetect(void);
int8_t SET_COMAT_DETECT(uint8_t *p_at_com,const uint8_t *at_com_backval,uint8_t send_data_count);
int8_t LoRaNode_Join6(uint8_t send_data_count,uint8_t join6_success_fail);
void LoRaNode_Response_Recv(LoRaNode_Timeout_Enable timeout_enable,LoRaNode_Timing_t max_timeout,uint8_t* str);



#endif //__(atcmd.h)