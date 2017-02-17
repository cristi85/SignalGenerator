#include "board.h"
#include "types.h"
#include "ADS1112_cfg.h"
#include "timeout.h"

/*! \brief Writes data from buffer.
    \param indata Pointer to data buffer
    \param bytes  Number of bytes to transfer
	\param slave_adr  Slave address on I2C bus
    \return 1 if successful, otherwise 0
 */
u8 I2C_WriteBytesNoSlaveReg(u8* databuff, u8 numbytes, u8 slave_adr)
{
  u8 i;
  u32 l_tstamp;
  
  l_tstamp = TIMER_1US_CNT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
  if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(I2C2, ADS1112_ADR, numbytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  
  //TXIS is set after Address and ACK bit is transmitted
  l_tstamp = TIMER_1US_CNT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
  if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
  
  if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
  {
    return 0;  // address not acknowledged by slave
  }
  
  /* Send data */
  for(i = 0; i < numbytes; i++)
  {
    I2C_SendData(I2C2, databuff[i]);
    l_tstamp = TIMER_1US_CNT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
    if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
    if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
    {
      return 0;  // data not acknowledged by slave
    }
  }
  
  //Wait for the stop flag to be set indicating
  //a stop condition has been sent
  l_tstamp = TIMER_1US_CNT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
  if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
  
  //Clear the stop flag for the next potential transfer
  I2C_ClearFlag(I2C2, I2C_FLAG_STOPF);
  
  return 1;
}

/*! \brief Reads data into buffer.
    \param data Pointer to data buffer
    \param bytes  Number of bytes to read
	\param slave_adr  Slave address on I2C bus
    \return 1 if successful, otherwise 0
 */
u8 I2C_ReadBytesNoSlaveReg(u8* data, u8 numbytes, u8 slave_adr)
{
  u8 i;
  u32 l_tstamp;
  
  l_tstamp = TIMER_1US_CNT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
  if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
  
  I2C_TransferHandling(I2C2, ADS1112_ADR, numbytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  //TXIS is set after Address and ACK bit is transmitted
  //while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET);
  if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
  {
    return 0;  // address not acknowledged by slave
  }
  
  for(i = 0; i < numbytes; i++)
  {
    l_tstamp = TIMER_1US_CNT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET && ((TIMER_1US_CNT - l_tstamp) < I2C_COMM_TOUT_US));
    if((TIMER_1US_CNT - l_tstamp) >= I2C_COMM_TOUT_US) return 0;
    data[i] = I2C_ReceiveData(I2C2);
  }
  return 1;
}

