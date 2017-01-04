#ifndef _DELAY_H_
#define _DELAY_H_

#include "board.h"
#include "types.h"

#define I2C_DELAY      (u8)2 
#define SCL_SDA_DELAY  (u8)2  


/* Name:  Delay_us - Macro to delay multiple of 1us 
   Param: us - size=u16
*/
#define DELAY_US(us) TIM14->ARR = us-1;                    \
                     TIM14->CNT = 0;                       \
                     TIM14->SR &= ~TIM_SR_UIF;             \
                     while((TIM14->SR & TIM_SR_UIF)==0){}  \
                     TIM14->SR &= ~TIM_SR_UIF;  

/* Blocking delay functions */
void delay_us(u16);

/* Non blocking delay functions */
/* If multiple functions are active at the same time, they must be different functions!!! */
/* If the same function is active in more than one place at the same time it will behave like a single function */
u8 delay_us_nonblocking1(u16 n_us);
u8 delay_us_nonblocking2(u16 n_us);

#endif