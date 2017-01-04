#include "board.h"
#include "types.h"
#include "delay.h"
#include "HD44780.h"

static u8 step_LCD_Home = 0;
static u8 step_LCD_GoTo = 0;
static u8 step_LCD_WriteString = 0;
static u8 step_LCD_Move_Cursor = 0;
static u8 step_LCD_Clear = 0;
static u8 step_LCD_Write = 0;
static u8 cnt_LCD_WriteString = 0;
static char _lcd_string[18] = " ";
/******************************************************************************
*                          Basic display functions                            *
******************************************************************************/
void LCD_Clock()
{
  LCD_EN(1);
  delay_us(10);
  LCD_EN(0);
}

u8 LCD_Write(u8 c) /* Duration 150us */
{
  if(step_LCD_Write == 0)
  {
    LCD_D7((u8)((c >> 7) & 0x01));
    LCD_D6((u8)((c >> 6) & 0x01));
    LCD_D5((u8)((c >> 5) & 0x01));
    LCD_D4((u8)((c >> 4) & 0x01));
    LCD_Clock();  // 4-bit only
    LCD_D7((u8)((c >> 3) & 0x01));
    LCD_D6((u8)((c >> 2) & 0x01));
    LCD_D5((u8)((c >> 1) & 0x01));
    LCD_D4((u8)((c >> 0) & 0x01));
    LCD_Clock();
    step_LCD_Write = 1;
  }
  else
  {
    if(delay_us_nonblocking1(100)) 
    {
      step_LCD_Write = 0;
      return 1;
    }
  }
  return 0;
}

void LCD_Initialize()
{
  LCD_RS(0);
  LCD_EN(0);

  LCD_D7(0);
  LCD_D6(0);
  LCD_D5(1);
  LCD_D4(1);
  LCD_Clock();
  delay_us(5000);

  LCD_Clock();
  delay_us(1000);

  LCD_Clock();
  delay_us(1000);

  LCD_D7(0);
  LCD_D6(0);
  LCD_D5(1);
  LCD_D4(0);
  LCD_Clock();
  delay_us(1000);

  //At this point device switches to 4 bit mode

  // FUNCTION SET				| 0  0  1  DL N  F  —  — |
  // BIN_OR_BIT_MASK:			{ 0  0  1  0  0  0  0  0 }
  // HEX_OR_BIT_MASK:			0x20
  while(!LCD_Write((((LCD_DL<<4)&0x10)+((LCD_N<<3)&0x08)+((LCD_F <<2)&0x04)) | 0x20));

  // DISPLAY ON/OFF CONTROL	| 0  0  0  0  1  D  C  B |
  // BIN_OR_BIT_MASK:			{ 0  0  0  0  1  0  0  0 }
  // HEX_OR_BIT_MASK:			0x08
  while(!LCD_Write(0x08));   //Display is off, cursor off, cursor blink off

  // CLEAR LCD DISPLAY
  while(!LCD_Write(0x01));   

  delay_us(4000);   //Wait for more than 3ms or until busy flag is clear

  // ENTRY MODE SET			| 0  0  0  0  0  1  ID S |
  // BIN_OR_BIT_MASK:			{ 0  0  0  0  0  1  0  0 }
  // HEX_OR_BIT_MASK:			0x04
  while(!LCD_Write((((LCD_ID<<1)&0x02)+(LCD_S&0x01)) | 0x04));

  //Initialization complete

  // DISPLAY ON/OFF CONTROL	| 0  0  0  0  1  D  C  B |
  // BIN_OR_BIT_MASK:			{ 0  0  0  0  1  0  0  0 }
  // HEX_OR_BIT_MASK:			0x08
  while(!LCD_Write((((LCD_D<<2)&0x04)+((LCD_C<<1)&0x02)+(LCD_B&0x01)) | 0x08));

  // CURSOR OR DISPLAY SHIFT	| 0  0  0  1  SC RL —  — |
  // BIN_OR_BIT_MASK:			{ 0  0  0  1  0  0  0  0 } 
  // HEX_OR_BIT_MASK:			0x10
  //LCD_Write((((LCD_SC<<3)&0x08)+((LCD_RL<<3)&0x04)) | 0x10);
}

/******************************************************************************
*                          Extended display functions                         *
******************************************************************************/
void LCD_WriteNumber(u32 num)
{
  LCD_RS(1);    // Data send
  if (num == 0) LCD_Write('0');
  else 
  {
    static u8 numstr[10];
    u8 CurPos=0;
    while (num != 0)
    {
      numstr[CurPos] = (u8)((u8)(num%10) + 48);
      num /= 10;
      CurPos++;
    }

    while (CurPos--) LCD_Write(numstr[CurPos]);
  }
}

void LCD_WriteByte(u8 num)
{
  LCD_RS(1);    // Data send
  if(((num & 0xF0) >> 4) < 10) LCD_Write(((num & 0xF0) >> 4) + 48);
  else LCD_Write(((num & 0xF0) >> 4) + 55);
  if((num & 0x0F) < 10) LCD_Write((num & 0x0F) + 48);
  else LCD_Write((num & 0x0F) + 55);
}

u8 LCD_WriteString(const char* s)
{
  switch(step_LCD_WriteString)
  {
  case 0:
    {
      u8 _cnt = 0;
      LCD_RS(1);  // Data send
      //copy string send by parameter to driver work string
      while(s[_cnt] && _cnt <= 16) {
        _lcd_string[_cnt] = s[_cnt];
        _cnt++;
      }
      step_LCD_WriteString = 1;
      break;
    }
  case 1:
    {
      if(_lcd_string[cnt_LCD_WriteString] == 0)
      {
        cnt_LCD_WriteString = 0;
        step_LCD_WriteString = 0;
        return 1;
      }
      else
      {
        if(_lcd_string[cnt_LCD_WriteString] != '\n') {
          step_LCD_WriteString = 3;
        }
        else {
          step_LCD_WriteString = 2;
        }
      }
      break;
    }
  case 2:
    {
      if(LCD_GoTo(0x40)) {
        cnt_LCD_WriteString++;
        step_LCD_WriteString = 1;
      }
      break;
    }
  case 3:
    {
      if(LCD_Write(_lcd_string[cnt_LCD_WriteString])) {
        cnt_LCD_WriteString++;
        step_LCD_WriteString = 1;
      }
      break;
    }
  default:break;
  }
  return 0;
}

u8 LCD_Clear(void)
{
  if(step_LCD_Clear == 0)
  {
    LCD_RS(0);    // Command send
    if(LCD_Write(0x01)) {
      LCD_RS(1);
      step_LCD_Clear = 1;
    }
  }
  else
  {
    if(delay_us_nonblocking1(2000)) {
      step_LCD_Clear = 0;
      return 1;
    }
  }
  return 0;
}

u8 LCD_Home(void)
{
  if(step_LCD_Home == 0)
  {
    LCD_RS(0);    // Command send
    if(LCD_Write(0x02)) {
      LCD_RS(1);
      step_LCD_Home = 1;
    }
  }
  else
  {
    if(delay_us_nonblocking1(2000)) {
      step_LCD_Home = 0;
      return 1;
    }
  }
  return 0;
}

u8 LCD_GoTo(u8 P)
{
  if(step_LCD_GoTo == 0)
  {
    LCD_RS(0);    // Command send
    if(LCD_Write((u8)(0x80+P))) {
      LCD_RS(1);  
      step_LCD_GoTo = 1;
    }
  }
  else
  {
    if(delay_us_nonblocking1(2000)) 
    {
      step_LCD_GoTo = 0;
      return 1;
    }
  }
  return 0;
}

u8 LCD_Move_Cursor(u8 row, u8 col)
{
  if(step_LCD_Move_Cursor == 0)
  {
    LCD_RS(0);    // Command send
    if(LCD_Write((u8)(0x80 + 0x40*(row-1) + (col-1)))) {
      LCD_RS(1);
      step_LCD_Move_Cursor = 1;
    }
  }
  else
  {
    if(delay_us_nonblocking1(2000)) 
    {
      step_LCD_Move_Cursor = 0;
      return 1;
    }
  }
  return 0;
}