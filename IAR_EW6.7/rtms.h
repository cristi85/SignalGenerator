#ifndef _RTMS_H_
#define _RTMS_H_

#include "board.h"
#include "types.h"

#define TIMER_1US_CNT  (TIM2->CNT)  /* Free running 1us timer counter */

typedef enum
{
  RunningTask_NoTask    = (u8)0x00,
  RunningTask_10ms      = (u8)0x01,
  RunningTask_100ms     = (u8)0x02,
  RunningTask_250ms     = (u8)0x04,
  RunningTask_500ms     = (u8)0x08,
  RunningTask_1000ms    = (u8)0x10,
  RunningTask_Bkg       = (u8)0x20,
  RunningTask_PID_500us = (u8)0x40
}RunningTask_t;

extern u32 start_task, end_task, total_task, cpuload_task;
extern u32 start_int, end_int, total_int, total_int_intask, cpuload_int;
extern RunningTask_t RunningTask;

void RTMS_CpuLoadCalculation(void);
u16 RTMS_GetCpuLoadCurrent(void);
u16 RTMS_GetCpuLoadMin(void);
u16 RTMS_GetCpuLoadMax(void);

#define RTMS_MeasureTaskStart(task) \
{ \
  RunningTask |= task; \
  start_task = TIMER_1US_CNT; \
} \

#define RTMS_MeasureTaskEnd(task) \
{ \
  end_task = TIMER_1US_CNT; \
  total_task += end_task - start_task; /* contains also interrupt time which has to be substracted */ \
  RunningTask &= (u8)(~task); \
} \

#define RTMS_MeasureIntStart \
{ \
  start_int = TIMER_1US_CNT; \
} \

#define RTMS_MeasureIntEnd \
{ \
  end_int = TIMER_1US_CNT; \
  total_int += end_int - start_int; \
  if(RunningTask != RunningTask_NoTask) { \
    total_int_intask += end_int - start_int; \
  } \
} \

#endif