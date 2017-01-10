/* Includes ------------------------------------------------------------------*/

#include "board.h"
#include "types.h"

#define TIMER_COUNTER (TIM16->CNT)

/* variables for delay_us function witch is blocking and has only one instance */
static volatile u16 timer_tstamp, timer_delta;

/* variables for delay_us_nonblocking functions witch are nonblocking and have their own variables */
static volatile u16 timernonblock_tstamp1, timernonblock_delta1;
static u8 timernonblock_step1 = 0;
static volatile u16 timernonblock_tstamp2, timernonblock_delta2;
static u8 timernonblock_step2 = 0;
/**
  * @brief  time delay in us unit
  * @param  n_us is how many us to delay
  * @retval None
  */
void delay_us(u16 n_us)
{
  timer_tstamp = TIMER_COUNTER;
  while(1)
  {
    timer_delta = TIMER_COUNTER - timer_tstamp;
    if(timer_delta >= n_us) break;
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
  if(timernonblock_step1 == 0)
  {
    timernonblock_tstamp1 = TIMER_COUNTER;
    timernonblock_step1 = 1;
  }
  else
  {
    timernonblock_delta1 = TIMER_COUNTER - timernonblock_tstamp1;
    if(timernonblock_delta1 >= n_us) {
      timernonblock_step1 = 0;
      return 1;
    }
  }
  return 0;
}

u8 delay_us_nonblocking2(u16 n_us)
{
  if(timernonblock_step2 == 0)
  {
    timernonblock_tstamp2 = TIMER_COUNTER;
    timernonblock_step2 = 1;
  }
  else
  {
    timernonblock_delta2 = TIMER_COUNTER - timernonblock_tstamp2;
    if(timernonblock_delta2 >= n_us) {
      timernonblock_step2 = 0;
      return 1;
    }
  }
  return 0;
}