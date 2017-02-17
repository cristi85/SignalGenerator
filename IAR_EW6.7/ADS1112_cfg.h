#ifndef _ADS1112_CFG_H_
#define _ADS1112_CFG_H_

#include "board.h"
#include "types.h"

#define ADS1112_ADR (u8)0x90   /* A1:A0 tied to GND */

#define TIMER_1US_CNT     (TIM2->CNT)
#define I2C_COMM_TOUT_US  (u32)(1000)   

u8 I2C_ReadBytesNoSlaveReg(u8*, u8, u8);
u8 I2C_WriteBytesNoSlaveReg(u8* databuff, u8 numbytes, u8 slave_adr);

#endif