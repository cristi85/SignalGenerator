/* Includes ------------------------------------------------------------------*/

#include "board.h"
#include "types.h"

/**
  Uses Timer 15, Timer15 clock must be activated before
  * @brief  time delay in 10us unit
  * @param  n_10us is how many 10us of time to delay
  * @retval None
  */
void delay_10us(u16 n_10us)
{
  /* Enable Counter */
  TIM_Cmd(TIM15, ENABLE);
  
  TIM_SetCounter(TIM15, 0);

  /* clear update flag */
  TIM_ClearITPendingBit(TIM15, TIM_IT_Update);

  while(n_10us--)
  {
    while(TIM_GetFlagStatus(TIM15, TIM_IT_Update) == RESET);
    TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
  }

  /* Disable Counter */
  TIM_Cmd(TIM15, DISABLE);
}

/**
  * @brief  delay for some time in 4us unit(not so accurate for small values for n_4us)
            Timer4 has to be configured with prescaler 8 (for 2MHz SYSCLK) and peripheral clock enabled before calling this function
            Timer4 will be incremented every 4us
            Delay range for this function is 4us to 1020us for n_4us of 1 to 255
  * @param n_26us is how many 26.31us of time to delay
  * @retval None
  */
/*void delay_4us(u8 n_4us)
{
  TIM4->CNTR = 3;
  TIM4->ARR = n_4us;
  TIM4->SR1 &= (u8)(~TIM4_FLAG_Update);   // clear update flag
  TIM4->CR1 |= TIM4_CR1_CEN;   // Enable Counter

  while((TIM4->SR1 & TIM4_FLAG_Update) == 0);
  TIM4->SR1 &= (u8)(~TIM4_FLAG_Update);
    
  TIM4->CR1 &= (u8)(~TIM4_CR1_CEN);   // Disable Counter
}*/
