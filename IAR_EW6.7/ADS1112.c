#include "board.h"
#include "types.h"
#include "ADS1112_cfg.h"
#include "errors.h"

#define CFG_ST_DRDY (u8)0x80
#define CFG_INP1    (u8)0x40
#define CFG_INP0    (u8)0x20
#define CFG_SC      (u8)0x10
#define CFG_DR1     (u8)0x08
#define CFG_DR0     (u8)0x04
#define CFG_PGA1    (u8)0x02
#define CFG_PGA0    (u8)0x01
/*
  Init external ADC
  Measurement from AIN0:AIN1
  Single Conversion mode
  16 bit resolution mode, 15 samples/s
  Set gain to 1
*/
u8 ADS1112_Init()
{
  u8 cfg = 0, status = 0, tmp[3];
  //Configure measurement channel: init to AIN0 AIN1
  //cfg |= CFG_INP1;   //set CFG_INP bits  INP1:INP0 - 00 -> AIN0 AIN1
  //cfg |= CFG_INP0;   //                  INP1:INP0 - 01 -> AIN2 AIN3
                       //                  INP1:INP0 - 10 -> AIN0 AIN3
                       //                  INP1:INP0 - 11 -> AIN1 AIN3
  //Configure conversion mode
  cfg |= CFG_SC;       //set CFG_SC bit    SC - 0 -> continuous conversion mode
                       //                  SC - 1 -> single conversion mode
  //Configure data rate
  cfg |= CFG_DR1;      //Data rate: DR1:DR0 - 11 -> 15SPS (Resolution 16bits) 
  cfg |= CFG_DR0;
  //Configure Gain Setting: init to 1
  //cfg |= CFG_PGA1;   //Gain setting: PGA1:PGA0 - 00 -> 1; 01 -> 2; 10 -> 4; 11 -> 8
  //cfg |= CFG_PGA0;
  status = I2C_WriteBytesNoSlaveReg(&cfg, 1, ADS1112_ADR);
  if(status == 0) 
  {
    Errors_SetError(ERROR_ADS1112);
    return 0;
  }
  else Errors_ResetError(ERROR_ADS1112);
  status = I2C_ReadBytesNoSlaveReg(tmp, 3, ADS1112_ADR);
  if(status == 0)
  {
    Errors_SetError(ERROR_ADS1112);
    return 0;
  }
  else Errors_ResetError(ERROR_ADS1112);
  if(cfg != tmp[2])
  {
    Errors_SetError(ERROR_ADS1112);
    return 0;
  }
  else Errors_ResetError(ERROR_ADS1112);
  return status;
}
/*

*/
u8 ADS1112_TriggerConversion()
{
  u8 status = 0, tmp[3];
  //Read existing CFG register
  status = I2C_ReadBytesNoSlaveReg(tmp, 3, ADS1112_ADR);
  if(status == 0) Errors_SetError(ERROR_ADS1112);
  else Errors_ResetError(ERROR_ADS1112);
  tmp[2] |= CFG_ST_DRDY;  //set CFG_ST_DRDY bit, conversion start from selected channel
  status = I2C_WriteBytesNoSlaveReg(&tmp[2], 1, ADS1112_ADR);
  if(status == 0) Errors_SetError(ERROR_ADS1112);
  else Errors_ResetError(ERROR_ADS1112);
  return 1;
}
/*
Set measurement channel for ADC and trigger conversion
channel: 0-set channel 0
         1-set channel 1
*/
u8 ADS1112_SetMeasurementChannel(u8 channel, u8 PGA)
{
  u8 cfg = 0, status = 0;
  switch(channel)
  {
    case 1:
      {
        //INP1:INP0 - 01 -> AIN2 AIN3
        cfg |= CFG_INP0;    
        break;
      }
    case 2:
      {
        //INP1:INP0 - 10 -> AIN0 AIN3
        cfg |= CFG_INP1;
		break;
      }
    case 3:
      {
        //INP1:INP0 - 11 -> AIN1 AIN3
        cfg |= CFG_INP1;
        cfg |= CFG_INP0;
		break;
      }
    default:
      {
        //choose channel 0
        break;
      }
  }
  switch(PGA)
  {
    //cfg |= CFG_PGA1;   //Gain setting: PGA1:PGA0 - 00 -> 1; 01 -> 2; 10 -> 4; 11 -> 8
    //cfg |= CFG_PGA0;
    case 2:
      {
        cfg |= CFG_PGA0;
        break;
      }
    case 4:
      {
        cfg |= CFG_PGA1;
        break;
      }
    case 8:
      {
        cfg |= CFG_PGA1;
        cfg |= CFG_PGA0;
        break;
      }
    default: break;
  }
  cfg |= CFG_SC;       //SC - 1 -> single conversion mode
  cfg |= CFG_DR1;      //Data rate: DR1:DR0 - 11 -> 15SPS (Resolution 16bits) 
  cfg |= CFG_DR0;
  cfg |= CFG_ST_DRDY;  //set CFG_ST_DRDY bit, conversion start from selected channel
  status = I2C_WriteBytesNoSlaveReg(&cfg, 1, ADS1112_ADR);
  if(status == 0)
  {
    Errors_SetError(ERROR_ADS1112);
    return 0;
  }
  else Errors_ResetError(ERROR_ADS1112);
  return status;
}
/*
  Read 16bit output register from ADC
  samp      - get signed sample from ADC range -32768:32767
  sampstat  - get status of read sample: 1 -> new data; 0 -> old data
*/
u8 ADS1112_GetSample(s16* samp, u8* sampstat)
{
  u8 status = 0, tmp[3];
  *sampstat = 0;
  status = I2C_ReadBytesNoSlaveReg(tmp, 3, ADS1112_ADR);
  if(status == 0)
  {
    Errors_SetError(ERROR_ADS1112);
    return 0;
  }
  else Errors_ResetError(ERROR_ADS1112);
  *samp = tmp[0];
  *samp <<= 8;
  *samp |= tmp[1];
  if(tmp[2] & CFG_ST_DRDY) *sampstat = 0;
  else *sampstat = 1;
  return status;
}
