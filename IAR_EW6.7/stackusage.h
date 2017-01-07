#ifndef _STACKUSAGE_H_
#define _STACKUSAGE_H_

#include "board.h"
#include "types.h"

void StackUsage_Init(void);
void StackUsage_CalcMaxUsage(void);
u8 StackUsage_GetMaxStackUsage(void);

#endif