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

#define VREFINT_CAL   (u32)0x1FFFF7BA

#define UART_CMD_1  (u8)0x01
#define UART_CMD_2  (u8)0x10
#define UART_CMD_3  (u8)0x02

typedef enum 
{
  Screen_ConstCurrent  = 0,
  Screen_ConstPower    = 1,
  Screen_ConstResistor = 2
}Screen_t;

/* Calibration Data */
#define STM_Clk_Src_HSI  (u8)0
#define STM_Clk_Src_HSE  (u8)1
extern u8 STM_Clk_Src;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const u16* ptr_VREFINT_CAL = (u16*)VREFINT_CAL;
u16 VrefINT_CAL; 

Screen_t Current_Screen = Screen_ConstCurrent;
u16 ADC_Conv_Tab_Avg[ADC_Scan_Channels];
/* Private function prototypes -----------------------------------------------*/
void TASK_RFCommand(void);
void TASK_UARTCommands(void);
/* Private functions ---------------------------------------------------------*/
#define LCD_CLEAR_ROW     "                "
#define LCD_CLEAR_HALFROW "        "
#define LCD_CLEAR_DUTY    "       "

static char lcd_row1[18] = "                \n";
static char lcd_row2[17] = LCD_CLEAR_ROW;

static u32 UmV, ImA;
static volatile u16 DACdata = 0;
static u32 Pot = 0, Pot_old = 0;

bool FLAG_RdCurrentSenOffset = TRUE;
u32 CurrentSenOffset = 0, CurrentSenOffset_acc = 0;
u8 cnt_RdCurrentSenOffset = 0;

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
      /* PA3, Vref, PA8, PA5 */
      /* Vref, Pot, I,   U */
      char strtmp[11];
      FLAG_250ms = FALSE;
      if(FLAG_RdCurrentSenOffset)
      {
        if(FLAG_ADC_NewData)
        {
          CurrentSenOffset_acc += ADC_Conv_Tab_Avg[2];
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
        if(FLAG_ADC_NewData)
        {
          ImA = ADC_Conv_Tab_Avg[2];
          if(ImA < CurrentSenOffset) { 
            ImA = 0;
          }
          else {
            ImA -= CurrentSenOffset;
          }
          /* Ima: 0 ... 571    */
          /* ImA: 0 ... 40000mA */
          if(ImA != 0) {
            ImA *= 70053;
            ImA /= 1000;
          }
          UmV = ADC_Conv_Tab_Avg[3];
          /* R1 = 4k7, R2 = 47k */
          /* UmV: 0 ... 4095    */
          /* UmV: 0 ... 36.3V */
          if(UmV != 0) {
            UmV *= 8865;
            UmV /= 1000;
          }
          Pot = (ADC_Conv_Tab_Avg[1] * 100) / 4096;
          if(Pot_old > 0 && Pot == 0) {
            LCD_Clear();
            LCD_Home();
            LCD_WriteString("Calibrating...");
            FLAG_RdCurrentSenOffset = TRUE;
          }
          Pot_old = Pot;
          DACdata = CurrentSenOffset + (u16)Pot;
          DAC_SetChannel1Data(DAC_Align_12b_R, DACdata);
          
          string_copy_noterm(lcd_row1, LCD_CLEAR_ROW);
          string_append_spaceterm(lcd_row1, "I=");
          string_append_spaceterm(lcd_row1, string_U32ToStr(ImA, strtmp));
          string_append_spaceterm(lcd_row1, "mA");
          string_append_spaceterm(&lcd_row1[8], "U=");
          string_append_spaceterm(&lcd_row1[8], string_U32ToStr(UmV, strtmp));
          string_append_spaceterm(&lcd_row1[8], "mV");
          
          string_copy_noterm(lcd_row2, LCD_CLEAR_ROW);
          string_append_spaceterm(lcd_row2, "Pot=");
          string_append_spaceterm(lcd_row2, string_U32ToStr(Pot, strtmp));
          string_append_spaceterm(lcd_row2, "%");
          string_append_spaceterm(&lcd_row2[8], "Cal=");
          string_append_spaceterm(&lcd_row2[8], string_U32ToStr(CurrentSenOffset, strtmp));
          
          LCD_Home();
          LCD_WriteString(lcd_row1);
          LCD_WriteString(lcd_row2);
          
          FLAG_ADC_NewData = FALSE;
        }
      }
    }
    
    /* ============== PRESS BTN FREQ INC ================= */
    if(BTN_MODE_DEB_STATE == BTN_PRESSED && BTN_MODE_DELAY_FLAG)
    {
      BTN_MODE_DELAY_FLAG = FALSE;
    } 
    
    /* ============== PRESS BTN FREQ DEC ================= */
    if(BTN_DEC_DEB_STATE == BTN_PRESSED && BTN_DEC_DELAY_FLAG)
    {
      BTN_DEC_DELAY_FLAG = FALSE; 
    }
    
    /* ============== PRESS BTN FREQ DUTY ================= */
    if(BTN_INC_DEB_STATE == BTN_PRESSED && BTN_INC_DELAY_FLAG)
    {
      BTN_INC_DELAY_FLAG = FALSE;
    }
    
    // ============= UART COMMAND RECEIVED ==============
    if(FLAG_UART_cmd_rcv)
    {
      switch(UART_CMD.CMD)
      {
        case UART_CMD_1:
        {
          //acknowledge CMD1
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x50);
          break;
        }
        case UART_CMD_2:
        {
          //acknowledge CMD2
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x60);
          break;
        }
        case UART_CMD_3:
        {
          //acknowledge CMD3
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
          USART_SendData(USART1, 0x70);
          break;
        }
        default:
        {
          //acknowledge command not recognized
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x8F);
          break;
        }
      }
      FLAG_UART_cmd_rcv = FALSE;
    }
    // ============= END UART COMMAND RECEIVED ==============
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
