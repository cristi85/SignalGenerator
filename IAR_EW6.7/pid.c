/*
PID controller module
*/

#include "board.h"
#include "types.h"


#define KP 0.005
#define KI 10
#define KD 0

#define PID_MUL        256
#define PID_PERIOD     0.0005
#define KP_MUL         (PID_MUL * KP)
#define KI_MUL         (PID_MUL * KI * PID_PERIOD)
#define KD_MUL         ((PID_MUL * KD) / PID_PERIOD)
#define INTEGRAL_MAX   (600)
#define INTEGRAL_MIN   (-600)
#define PID_MAX        (700)
#define PID_MIN        (0)
#define PID_OUT_OFFSET (2040)

volatile s32 _P, _I, _D, _pid, _out;
s32 integral = 0, derivative = 0;
s32 /*error = 0,*/ error_old = 0;

void PID_Init()
{
  error_old = 0;
  integral = 0;
}

u32 PID_Update(s32 error, u32 sen_offset)
{
  integral += error;
  derivative = error - error_old;
  if(integral > INTEGRAL_MAX) integral = INTEGRAL_MAX;
  else if(integral < INTEGRAL_MIN) integral = INTEGRAL_MIN;
  
  _P = KP_MUL * error;
  _I = KI_MUL * integral;
  _D = KD_MUL * derivative;
  
  _pid = _P + _I + _D;
  _pid /= PID_MUL;
  if(_pid > PID_MAX) _pid = PID_MAX;
  else if(_pid < PID_MIN) _pid = PID_MIN;
  error_old = error;
  _out = (u32)_pid + sen_offset;
  return _out;
}