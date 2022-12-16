

#include "main.h"
#include "stm32wlxx_it.h"

extern RTC_HandleTypeDef hrtc;
extern SUBGHZ_HandleTypeDef hsubghz;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;



void NMI_Handler(void)
{
  while (1)
  {
  }
}


void HardFault_Handler(void)
{

  while (1)
  {

  }
}


void MemManage_Handler(void)
{

  while (1)
  {

  }
}


void BusFault_Handler(void)
{

  while (1)
  {

  }
}


void UsageFault_Handler(void)
{

  while (1)
  {

  }
}


void SVC_Handler(void)
{

}


void DebugMon_Handler(void)
{

}


void PendSV_Handler(void)
{

}


void SysTick_Handler(void)
{

}

/******************************************************************************/
/* STM32WLxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32wlxx.s).                    */
/******************************************************************************/


void TAMP_STAMP_LSECSS_SSRU_IRQHandler(void)
{

  HAL_RTCEx_SSRUIRQHandler(&hrtc);

}


void EXTI0_IRQHandler(void)
{

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

}


void EXTI1_IRQHandler(void)
{

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

}


void DMA1_Channel5_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_usart2_tx);

}


void USART2_IRQHandler(void)
{

  HAL_UART_IRQHandler(&huart2);

}


void RTC_Alarm_IRQHandler(void)
{

  HAL_RTC_AlarmIRQHandler(&hrtc);

}


void SUBGHZ_Radio_IRQHandler(void)
{

  HAL_SUBGHZ_IRQHandler(&hsubghz);

}


