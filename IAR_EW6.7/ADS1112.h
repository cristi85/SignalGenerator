#ifndef _ADS1112_H_
#define _ADS1112_H_

#include "board.h"
#include "types.h"

u8 ADS1112_Init(void);
u8 ADS1112_TriggerConversion(void);
u8 ADS1112_SetMeasurementChannel(u8, u8);
u8 ADS1112_GetSample(s16*, u8*);

#endif