#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "types.h"

#define ERROR_ADS1112    (u8)0
#define ERROR_CLOCK_SRC  (u8)1
#define ERROR_CLOCK_FREQ (u8)2

#define ERRORS (u8)ERROR_CLOCK_FREQ + 1

void Errors_Init(void);
void Errors_SetError(u8);
void Errors_ResetError(u8);
_Bool Errors_CheckError(u8);
_Bool Errors_IsError(void);

#endif