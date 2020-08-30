#ifndef __LOWPOWER_H
#define __LOWPOWER_H

#include "stm32l4xx_hal.h"
#include "lorawan.h"

uint8_t Time_Out_Break(uint32_t MAX_time);
void LowPower_Init(uint16_t time);
void LowPower_Handler(Node_Info *LoRa_temp);
void LoRaNode_Go_To_Sleep(uint16_t d_time);
void LoRaNode_Send_To_Sleep(void);
void Set_RTC_WakeUp(uint16_t D_time);
void LPUART_CharReception_Callback(void);
void SystemRecovery(void);
void RecoveryClock(void);

#endif
