/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Pinout Configuration
*/
 void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
		
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_LPUART1_CLK_ENABLE();
	//-----------------------------LED PA10 PB1 PD7 PE0 ---------------------		
	GPIO_InitStruct.Pin = LedPin_WAKE;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_WAKE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LedPin_MODE;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_MODE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LedPin_STAT;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_STAT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LedPin_BUSY;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_BUSY, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LedGpio_WAKE, LedPin_WAKE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_MODE, LedPin_MODE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_STAT, LedPin_STAT, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_BUSY, LedPin_BUSY, GPIO_PIN_SET);
	
	//-------------Configure GPIO pins : PC3 PC4 PC5 PA0 PA1------------------------
	GPIO_InitStruct.Pin = Pin_WAKE;                      //wake
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_WAKE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = Pin_MODE;                      //MODE
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_MODE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Pin_BUSY;                      //BUSY
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_BUSY, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin =  Pin_STAT;                     //STAT
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_STAT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = Pin_RST;                       //RST
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_RST, &GPIO_InitStruct);

	HAL_GPIO_WritePin(Gpio_WAKE, Pin_WAKE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Gpio_MODE, Pin_MODE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Gpio_RST, Pin_RST, GPIO_PIN_SET);
	
	//---------------------------- K1 K2  PC13 PE6------------------------------
	GPIO_InitStruct.Pin = Pin_KEY1;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_KEY1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Pin_KEY2;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_KEY2, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
 	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);	
}


void gpio_into_sleep(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   
  /* GPIO Ports Clock Enable */
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	__HAL_RCC_GPIOA_CLK_DISABLE();
//	__HAL_RCC_GPIOB_CLK_DISABLE();
//	__HAL_RCC_GPIOD_CLK_DISABLE();
//	__HAL_RCC_GPIOE_CLK_DISABLE();
	//-----------------------------LED PA10 PB1 PD7 PE0 ---------------------  
	GPIO_InitStruct.Pin = LedPin_WAKE;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_WAKE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LedPin_MODE;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_MODE, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LedPin_STAT;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_STAT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LedPin_BUSY;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_BUSY, &GPIO_InitStruct);
 
/********************      LSD4WN-2N717M9      ****************************/
/*Configure GPIO pins : PC3 PC4 PC5 PA0 PA1*/
	GPIO_InitStruct.Pin = Pin_BUSY;                      //BUSY
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_BUSY, &GPIO_InitStruct);
 
	GPIO_InitStruct.Pin =  Pin_STAT;                     //STAT
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_STAT, &GPIO_InitStruct);  
// 
	//---------------------------- K1 K2  PC13 PE6------------------------------

//	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
 
//	GPIO_InitStruct.Pin = Pin_KEY1;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
////	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	HAL_GPIO_Init(Gpio_KEY1, &GPIO_InitStruct);
    
    
	GPIO_InitStruct.Pin = Pin_KEY2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_KEY2, &GPIO_InitStruct);
    
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
 
}

void gpio_into_wakeup(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   
//    __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	//-----------------------------LED PA10 PB1 PD7 PE0 ---------------------
 	GPIO_InitStruct.Pin = LedPin_WAKE;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_WAKE, &GPIO_InitStruct);
    
	GPIO_InitStruct.Pin = LedPin_MODE;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_MODE, &GPIO_InitStruct);
    
	GPIO_InitStruct.Pin = LedPin_STAT;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_STAT, &GPIO_InitStruct);
    
	GPIO_InitStruct.Pin = LedPin_BUSY;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LedGpio_BUSY, &GPIO_InitStruct);
    
	HAL_GPIO_WritePin(LedGpio_WAKE, LedPin_WAKE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_MODE, LedPin_MODE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_STAT, LedPin_STAT, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LedGpio_BUSY, LedPin_BUSY, GPIO_PIN_SET);
 
 
/********************      LSD4WN-2N717M9      ****************************/
/*Configure GPIO pins : PC3 PC4 PC5 PA0 PA1*/
	GPIO_InitStruct.Pin = Pin_BUSY;                      //BUSY
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_BUSY, &GPIO_InitStruct);
 
	GPIO_InitStruct.Pin =  Pin_STAT;                     //STAT
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_STAT, &GPIO_InitStruct);
 
	//---------------------------- K1 K2  PC13 PE6------------------------------
//	GPIO_InitStruct.Pin = Pin_KEY1;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
////	HAL_GPIO_Init(Gpio_KEY1, &GPIO_InitStruct);
    
	GPIO_InitStruct.Pin = Pin_KEY2;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Gpio_KEY2, &GPIO_InitStruct);
 
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
//    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
