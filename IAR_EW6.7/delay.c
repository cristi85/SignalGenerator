/* Includes ------------------------------------------------------------------*/

#include "board.h"
#include "types.h"

/* variables for delay_us function witch is blocking and has only one instance */
static volatile u16 t15_tstamp, t15_delta;

/* variables for delay_us_nonblocking functions witch are nonblocking and have their own variables */
static volatile u16 t15nonblock_tstamp1, t15nonblock_delta1;
static u8 t15nonblock_step1 = 0;
static volatile u16 t15nonblock_tstamp2, t15nonblock_delta2;
static u8 t15nonblock_step2 = 0;
/**
  Uses Timer 15
  * @brief  time delay in us unit
  * @param  n_us is how many us to delay
  * @retval None
  */
void delay_us(u16 n_us)
{
  t15_tstamp = TIM15->CNT;
  while(1)
  {
    t15_delta = TIM15->CNT - t15_tstamp;
    if(t15_delta >= n_us) break;
  }
}

/**
  Non blocking delay function
  * @brief  time delay in us unit
  * @param  n_us is how many us to delay
  * @retval Function returns 0 when delay time has not passed and 1 when delay time has passed
  */
u8 delay_us_nonblocking1(u16 n_us)
{
  if(t15nonblock_step1 == 0)
  {
    t15nonblock_tstamp1 = TIM15->CNT;
    t15nonblock_step1 = 1;
  }
  else
  {
    t15nonblock_delta1 = TIM15->CNT - t15nonblock_tstamp1;
    if(t15nonblock_delta1 >= n_us) {
      t15nonblock_step1 = 0;
      return 1;
    }
  }
  return 0;
}

u8 delay_us_nonblocking2(u16 n_us)
{
  if(t15nonblock_step2 == 0)
  {
    t15nonblock_tstamp2 = TIM15->CNT;
    t15nonblock_step2 = 1;
  }
  else
  {
    t15nonblock_delta2 = TIM15->CNT - t15nonblock_tstamp2;
    if(t15nonblock_delta2 >= n_us) {
      t15nonblock_step2 = 0;
      return 1;
    }
  }
  return 0;
}