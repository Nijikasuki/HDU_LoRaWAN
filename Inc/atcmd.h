#ifndef __ATCMD_H__
#define __ATCMD_H__
#include "stdint.h"



#define ATCMD_MAX_LENGTH 256
#define ATCMD_MAX_FAILCOUNT 128
#define RETURN_OK "OK"
#define RETURN_FALSE "false"


#define STD_CN470        0    //��׼470

#define CN470P_SFREQ     0    //����470ͬƵ

#define CN470P_DFREQ     1    //����470��Ƶ

/*--------------------------ϵͳ��ʱʱ��궨��--------------------------------*/
#define LORANODE_DELAY(x) HAL_Delay(x)

#define TIMEOUT_COUNTER(x) Time_Out_Break(x)

/*RTC��ʱ����ϵͳʱ�䣬��λs*/
#define RTC_Wakeup_Time  30

/*ģ�������ʱ��ö��*/
typedef enum       //��λ��ms
{
	PowerOn_Init_Time = 500,                    //�ϵ��ʼ��ʱ��
	Wake_PullLow_Time = 10,                     //wake����ʱ��
	Wake_PullHigh_Time = 10,                    //wake����ʱ��
	ATCmd_Response_Max_Timeout = 2000,          //ָ����Ӧ���ʱʱ��
	Commond_Mode_Stime = 10,                    //�л���ָ��ģʽʱ��
	Transparent_Mode_Stime = 30,                //�л���͸��ģʽʱ��
	Factory_Init_Time = 1500,                   //�ָ��������ú��ʼ��ʱ��
	Reset_Init_Time = 150,                      //��λ���ʼ��ʱ��
	Busy_PullLow_CmdToTrans_Timeout = 1000,     //ָ����͸���ȴ�busy���ͳ�ʱʱ��
	Busy_PullHigh_Join_Max_Timeout = 240000,    //�����ȴ�busy�������ʱʱ��
	Busy_Wakeup_PullHigh_Timeout = 1000,        //���Ѻ�ȴ�busy���߳�ʱʱ��
	Busy_SendDataCmt_PullLow_Timeout = 1000,    //�������ݷ�����ɺ󣬵ȴ�busy���ͳ�ʱʱ��
	Busy_DataCommunicateCmt_PullHigh = 60000,   //����ͨ����ɺ󣬵ȴ�busy���߳�ʱʱ��
	DataRecvive_Max_Timeout = 2000,              //����ͨ����ɺ󣬽��������������ȴ�ʱ��,�����ڸ���������
	Debug_JoinSuccess_Timeout = 300,            //���debug�򿪣������ɹ������һ����־��ӡ��ʱʱ��
	Reset_Init_Timeout = 300,                   //���debug�򿪣�reset֮���ӡ��ʼ����־��ʱʱ��
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