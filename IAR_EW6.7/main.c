/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"
#include "config.h"
#include "stm32f0xx_it.h"
#include "delay.h"
#include "errors.h"
#include "config.h"
#include "timeout.h"
#include "hd44780.h"
#include "string.h"

#define STM_Clk_Src_HSI  (u8)0
#define STM_Clk_Src_HSE  (u8)1
extern u8 STM_Clk_Src;

/* Vref INT CAL */
const u16* ptr_VREFINT_CAL = (u16*)0x1FFFF7BA;
u16 VrefINT_CAL; 

/* LCD MACROS and variables */
#define LCD_CLEAR_ROW     "                "
#define LCD_CLEAR_HALFROW "        "
#define LCD_CLEAR_DUTY    "       "
static char lcd_row1[18] = "                \n";
static char lcd_row2[17] = LCD_CLEAR_ROW;

#define POWER_LIMIT   (u32)30000  /* mili Watts */
#define CURRENT_LIMIT (u32)5000   /* mili Amps */
static u32 UmV, ImA, PowermW;
static u16 DACdata = 0;
static u16 Requested_Current = 0, Requested_Current_old = 0;
bool FLAG_Power_limit = FALSE;
bool FLAG_Current_limit = FALSE;

/* LEM current sensor offset variables */
u32 CurrentSenOffset = 0, CurrentSenOffset_acc = 0;
u8 cnt_RdCurrentSenOffset = 0;
bool FLAG_RdCurrentSenOffset = TRUE;

int main(void)
{
  VrefINT_CAL = *ptr_VREFINT_CAL;
  Config();
  Errors_Init();
  
  SystemCoreClockUpdate();
  
  LCD_Initialize();
  LCD_Clear();
  LCD_Home();
  LCD_WriteString("Calibrating...");
  
  while (1)
  {
    /* ============== CYCLIC ENTRIES ================= */
    if(FLAG_10ms)
    {
      /* Power and current limit check */
      FLAG_10ms = FALSE;
      if(FLAG_ADC_NewData)
      {
        FLAG_ADC_NewData = FALSE;
        ImA = ADC_CURRENT;
        if(ImA < CurrentSenOffset) { 
          ImA = 0;
        }
        else {
          ImA -= CurrentSenOffset;
        }
        /* LEM HLSR 40-P/SP33 (40A) -> 11.5mV/A ; ADC LSB (12bit) -> 70.058mA */
        if(ImA != 0) {
          ImA *= 70058;
          ImA /= 1000;
        }
        UmV = ADC_VOLTAGE;
        /* R1 = 4k7, R2 = 47k */
        /* UmV: 0 ... 4095    */
        /* UmV: 0 ... 36.3V */
        if(UmV != 0) {
          UmV *= 88645;
          UmV /= 10000;
        }
        PowermW = ImA * UmV;
        PowermW /= 1000;
        if(PowermW > POWER_LIMIT) {
          FLAG_Power_limit = TRUE;
        }
        else {
          DACdata = CurrentSenOffset + Requested_Current;
          DAC_SetChannel1Data(DAC_Align_12b_R, DACdata);
        }
      }
    }
    if(FLAG_1000ms)
    {
      FLAG_1000ms = FALSE;
    }
    
    if(FLAG_500ms)
    {
      FLAG_500ms = FALSE;
    }
    
    if(FLAG_250ms)
    {
      char strtmp[11];
      FLAG_250ms = FALSE;
      if(FLAG_RdCurrentSenOffset)
      {
        if(FLAG_ADC_NewData)
        {
          CurrentSenOffset_acc += ADC_CURRENT;
          FLAG_ADC_NewData = FALSE;
          cnt_RdCurrentSenOffset++;
          if(cnt_RdCurrentSenOffset >= 16)
          {
            CurrentSenOffset = CurrentSenOffset_acc / 16;
            cnt_RdCurrentSenOffset = 0;
            CurrentSenOffset_acc = 0;
            FLAG_RdCurrentSenOffset = FALSE;
          }
        }
      }
      else
      {
        string_copy_noterm(lcd_row1, LCD_CLEAR_ROW);
        string_append_spaceterm(lcd_row1, "I=");
        string_append_spaceterm(lcd_row1, string_U32ToStr(ImA, strtmp));
        string_append_spaceterm(lcd_row1, "mA");
        string_append_spaceterm(&lcd_row1[8], "U=");
        string_append_spaceterm(&lcd_row1[8], string_U32ToStr(UmV, strtmp));
        string_append_spaceterm(&lcd_row1[8], "mV");
        
        string_copy_noterm(lcd_row2, LCD_CLEAR_ROW);
        string_append_spaceterm(lcd_row2, "Pot=");
        string_append_spaceterm(lcd_row2, string_U32ToStr(Requested_Current, strtmp));
        string_append_spaceterm(lcd_row2, "%");
        string_append_spaceterm(&lcd_row2[8], "Cal=");
        string_append_spaceterm(&lcd_row2[8], string_U32ToStr(CurrentSenOffset, strtmp));
        
        LCD_Home();
        LCD_WriteString(lcd_row1);
        LCD_WriteString(lcd_row2);
      }
    }
    
    /* ============== PRESS BTN INC ================= */
    if(BTN_INC_DEB_STATE == BTN_PRESSED && BTN_INC_DELAY_FLAG)
    {
      BTN_INC_DELAY_FLAG = FALSE;
      if(Requested_Current < U16_MAX) Requested_Current++;
    }
    
    /* ============== PRESS BTN DEC ================= */
    if(BTN_DEC_DEB_STATE == BTN_PRESSED && BTN_DEC_DELAY_FLAG)
    {
      BTN_DEC_DELAY_FLAG = FALSE;
      if(Requested_Current > 0) Requested_Current--;
    }
    
    /* ============== PRESS BTN MODE ================= */
    if(BTN_MODE_DEB_STATE == BTN_PRESSED && BTN_MODE_DELAY_FLAG)
    {
      BTN_MODE_DELAY_FLAG = FALSE;
    }
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
