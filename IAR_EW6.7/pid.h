#ifndef _PID_H_
#define _PID_H_

#include "board.h"
#include "types.h"

void PID_Init(u32 sen_offset);
u32 PID_Update(s32 error);
u32 PID_Update2(s32 error);

#endif