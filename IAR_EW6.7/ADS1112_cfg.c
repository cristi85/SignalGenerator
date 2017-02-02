#include "board.h"
#include "types.h"
#include "ADS1112_cfg.h"

/*! \brief Writes data from buffer.
    \param indata Pointer to data buffer
    \param bytes  Number of bytes to transfer
	\param slave_adr  Slave address on I2C bus
    \return 1 if successful, otherwise 0
 */
u8 I2C_WriteBytesNoSlaveReg(u8* databuff, u8 numbytes, u8 slave_adr)
{
  u8 i;
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET);
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(I2C2, ADS1112_ADR, numbytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  
  //TXIS is set after Address and ACK bit is transmitted
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET);
  
  if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
  {
    return 0;  // address not acknowledged by slave
  }
  
  /* Send data */
  for(i = 0; i < numbytes; i++)
  {
    I2C_SendData(I2C2, databuff[i]);
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET);
    if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
    {
      return 0;  // data not acknowledged by slave
    }
  }
  
  //Wait for the stop flag to be set indicating
  //a stop condition has been sent
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET);
  
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
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET);
  
  I2C_TransferHandling(I2C2, ADS1112_ADR, numbytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  //TXIS is set after Address and ACK bit is transmitted
  //while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET);
  
  if(I2C_GetFlagStatus(I2C2, I2C_FLAG_NACKF) == SET)
  {
    return 0;  // address not acknowledged by slave
  }
  
  for(i = 0; i < numbytes; i++)
  {
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET); 
    data[i] = I2C_ReceiveData(I2C2);
  }
  return 1;
}

