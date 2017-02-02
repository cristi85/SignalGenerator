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

volatile s32 _P, _I, _D, _pid, _out;
s32 integral = 0, derivative = 0;
s32 /*error = 0,*/ error_old = 0;
u32 current_sensor_offset = 0, _out_zero = 0;

u32 correction = 0;
bool flag_error_negative = FALSE;

void PID_Init(u32 sen_offset)
{
  error_old = 0;
  integral = 0;
  current_sensor_offset = sen_offset;
  _out_zero = sen_offset - 4; /* substract a small nuber from sensor offset to ensure a zero output command */
}

u32 PID_Update(s32 error)
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
  _out = _out_zero + (u32)_pid;
  return _out;
}

u32 PID_Update2(s32 error)
{
  if(error > 0) _out++;
  else if(error < 0) _out--;
  return _out;
}