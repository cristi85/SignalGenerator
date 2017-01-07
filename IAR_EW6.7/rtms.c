/*
Runtime measurement module
Task runtime measurement
CPU load measurement
*/

#include "board.h"
#include "types.h"
#include "rtms.h"

#define CPULOAD_MEASUREMENT_CYCLE_US  (u32)1000000  /* in us - CPU Load calculation is done once every second*/  
#define CPULOAD_RESOLUTION            (u32)1000

u32 start_task, end_task, total_task = 0, cpuload_task = 0;
u32 start_int, end_int, total_int = 0, total_int_intask = 0, cpuload_int = 0;
static u32 cpuload_temp;
static u16 cpuload = 0, cpuload_min = U16_MAX, cpuload_max = 0;
RunningTask_t RunningTask = RunningTask_NoTask;

void RTMS_CpuLoadCalculation()
{
  /* CPU load calculation */
  total_task -= total_int_intask; /* substract from task time, interrupt time that occured during task runtime */
  cpuload_task = (total_task * CPULOAD_RESOLUTION) / CPULOAD_MEASUREMENT_CYCLE_US;
  cpuload_int = (total_int * CPULOAD_RESOLUTION) / CPULOAD_MEASUREMENT_CYCLE_US;
  total_task = 0;
  total_int = 0;
  total_int_intask = 0;
  cpuload_temp = cpuload_task + cpuload_int;
  if(cpuload_temp <= 1000) cpuload = (u16)cpuload_temp;
  else cpuload = 1000;
  if(cpuload_max < cpuload) cpuload_max = cpuload;
  if(cpuload_min > cpuload) cpuload_min = cpuload;
}

u16 RTMS_GetCpuLoadCurrent()
{
  return cpuload;
}

u16 RTMS_GetCpuLoadMin()
{
  return cpuload_min;
}

u16 RTMS_GetCpuLoadMax()
{
  return cpuload_max;
}
