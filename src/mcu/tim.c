/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef TIM6_InitStruct;


/* TIM6 init function */
//time 范围：0~6000
void Tim6_Conf(uint16_t time)
{
	TIM6_InitStruct.Instance = TIM6;
	uint32_t uwPrescalerValue = (uint32_t) (SYSCLOCK_FREQ/BASE_TIM_10000HZ - 1); //Compute the prescaler value to have TIMx counter clock equal to 10 Hz
	TIM6_InitStruct.Init.Period = time*10-1;
	TIM6_InitStruct.Init.Prescaler = uwPrescalerValue;
	TIM6_InitStruct.Init.ClockDivision = 0;
	TIM6_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&TIM6_InitStruct) != HAL_OK)
	{
		while(1){}
	}
}

//定时器使能
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance == TIM6)
    {
         __HAL_RCC_TIM6_CLK_ENABLE();            
         HAL_NVIC_SetPriority(TIM6_IRQn,1,3);    
         HAL_NVIC_EnableIRQ(TIM6_IRQn);     
	}
}

//定时器失能
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
    __HAL_RCC_TIM6_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM6_IRQn);       
}





/* USER CODE END 1 */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
