#include "board.h"
#include "types.h"
#include "calibdata.h"
#include "stm32f0xx_flash.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

void CalibData_Update()
{
  /* Store the address and the size of the ram calibration section */
  u32 cal_init_begin = (u32)__section_begin("CALINIT");
  u32 *cal_begin = __section_begin("CAL");
  u32 size = __section_size("CAL");
      
  /* Unlock the Flash Bank1 Program Erase controller */  
  FLASH_Unlock();
  
  /* Set latency to 1 clock cycles */
  FLASH_SetLatency(FLASH_Latency_1);

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);	

  /* Erase the CALINIT page */
  FLASHStatus = FLASH_ErasePage(cal_init_begin);

  /* Program Flash Bank1 */
  u32 Address = cal_init_begin;
  while((Address < (cal_init_begin+size)) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, *cal_begin);
    cal_begin++;
    Address+=4;
  }

  FLASH_Lock();
  
  /* Check the correctness of written data */
  Address = cal_init_begin;
  cal_begin = __section_begin("CAL");

  while((Address < (cal_init_begin+size)) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != *cal_begin)
    {
      MemoryProgramStatus = FAILED;
    }
    cal_begin++;
    Address += 4;
  }
}