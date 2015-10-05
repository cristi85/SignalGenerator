#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "types.h"

#define ERROR_ADC_INIT               (u8)0

#define ERRORS (u8)ERROR_ADC_INIT + 1

void Errors_Init(void);
void Errors_SetError(u8);
void Errors_ResetError(u8);
_Bool Errors_CheckError(u8);
_Bool Errors_IsError(void);

#endif