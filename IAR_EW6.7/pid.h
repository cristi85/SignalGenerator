#ifndef _PID_H_
#define _PID_H_

#include "board.h"
#include "types.h"

u32 PID_Update(s32 error, u32 sen_offset);

#endif