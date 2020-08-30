#include "lowpower.h"
#include "usart.h"
#include "main.h"
//#include "MPL3115.h"
#include "app.h"
#include "rtc.h"
#include "atcmd.h"
#include "gpio.h"


extern RTC_HandleTypeDef hrtc;

extern uint8_t Send_Sate_Sign;
uint8_t TimeOut_Sign = 0;


void Set_RTC_WakeUp(uint16_t D_time)
{
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, D_time, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	
	HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 1, 0);//??rtc?????
	HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);    
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
}


void LowPower_Init(uint16_t time)
{

    LoRaNode_Mode_Set(MODE_TRANSPARENT);
    gpio_into_sleep();
    debug_printf("mode pin level is %d\r\n",LORANODE_MODE_STATUS);
	LoRaNode_Wake_Sleep(MODE_SLEEP);
    Set_RTC_WakeUp(time);
    
  /* Select HSI as system clock source after Wake Up from Stop mode */
	__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
	
//	HAL_Delay(50);
	
	SysTick->CTRL = 0x00;//关闭定时器
    SysTick->VAL = 0x00;//清空val,清空定时器
	
	/* Enter Stop  mode */
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);       
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();   
}

void LoRaNode_Go_To_Sleep(uint16_t d_time)
{
	debug_printf("--> System enter Regular sleep\r\n");
    //设置RTC唤醒时间，时间10s
	LowPower_Init(d_time);
        
	SystemRecovery();
	debug_printf("--> System recovery\r\n");
}

void LoRaNode_Send_To_Sleep(void)
{
	debug_printf("--> System enter sending sleep\r\n");
//	PrepareLPUARTToStopMode();
        
//	Busy_Gpio_exit();
//	LowPower_Init();
        
	SystemRecovery();
	debug_printf("--> System recovery\r\n");
}

void SystemRecovery(void)
{
	SystemClock_Config();
    gpio_into_wakeup();
//	HAL_UART_MspInit(&hlpuart1);
//	usart2_clear_it();
//	Clear_UART1_IT();
//	lpusart1_clear_it();
	
//	HAL_Delay(10);
//    gpio_into_wakeup();
}

void RecoveryClock(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
  	RCC_ClkInitTypeDef RCC_ClkInitStruct;
  	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/* ??PWR???? */
    __PWR_CLK_ENABLE();

    /**Configure the main internal regulator output voltage 
    */
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	    /**Initializes the CPU, AHB and APB busses clocks 
	    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI ;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF ;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON ;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON ;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks */
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1 |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

}


//---------------------------?? LPUART ?????? -----------------------------
void LPUART_CharReception_Callback(void) 
{
	if(Send_Sate_Sign == 1)
	{
		Send_Sate_Sign = 12;
	}
}

