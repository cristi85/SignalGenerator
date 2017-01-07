/*
Maximum stack usage measurement module
*/

#include "board.h"
#include "types.h"

#define SP_VALUE_FLASH (u32)0x08000000
/* Get Stack pointer */
extern const u32 __ICFEDIT_size_cstack__;  //linker symbol with configured stack size
#define STACK_SIZE  (u32)(&__ICFEDIT_size_cstack__)
static u32 StackAddressTop;
static u32 StackAddresBottom;
static u32 current_SP;
static u32 MaxStackUsage_temp = 0;
static u8  MaxStackUsage = 0;

void StackUsage_Init()
{
  StackAddressTop = *((u32*)SP_VALUE_FLASH);
  StackAddresBottom = StackAddressTop - STACK_SIZE;
}

void StackUsage_CalcMaxUsage()
{
  /* Max STACK consumption calculation */
  /* Search for the first stack value different than pattern */
  current_SP = StackAddresBottom;
  while(current_SP <= StackAddressTop)
  {
    if(*(u32*)current_SP != (u32)0xABCDABCD)
    {
      MaxStackUsage_temp = (current_SP - StackAddresBottom)*100 / STACK_SIZE;
      MaxStackUsage_temp = 100 - MaxStackUsage_temp;
      MaxStackUsage = (u8)MaxStackUsage_temp;
      break;
    }
    current_SP += 4;
  }
}

u8 StackUsage_GetMaxStackUsage()
{
  return MaxStackUsage;
}