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
const struct CalibData CAL = 
  {
    100,    // LCD Backlight Brightness
    16      //number of samples when averaging ADC measured data
  };
#define STM_Clk_Src_HSI  (u8)0
#define STM_Clk_Src_HSE  (u8)1
extern u8 STM_Clk_Src;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const u16* ptr_VREFINT_CAL = (u16*)VREFINT_CAL;
u16 VrefINT_CAL; 

#define LCD_CLEAR_ROW     "                "
#define LCD_CLEAR_HALFROW "        "
#define LCD_CLEAR_DUTY    "       "

static char lcd_row1[18] = "                \n";
static char lcd_row2[17] = LCD_CLEAR_ROW;

#define PWM_PERIOD_START  (u16)1000
#define PWM_ONTIME_START  (u16)500
#define PWM_DUTY_START    (u16)500     //50% duty cycle
#define PWM_TBASE_START   (u16)48      // 48Mhz/48=1us timebase

Screen_t Current_Screen = Screen_ConstCurrent;
u16 ADC_Conv_Tab_Avg[ADC_Scan_Channels];
/* Private function prototypes -----------------------------------------------*/
void TASK_RFCommand(void);
void TASK_UARTCommands(void);
/* Private functions ---------------------------------------------------------*/
typedef enum
{
  Polarity_Positive = (u8)0x01,
  Polarity_Negative = (u8)0x02
}Pwm_Polarity_t;
u16 analog_sig_freq = 120;
u16 analog_sig_freq_old = 0;
u16 pwm_period = PWM_PERIOD_START;
u16 pwm_period_old = PWM_PERIOD_START;
u32 pwm_period_real = 0;
u32 pwm_ontime_real = 0;
u16 pwm_sig_pulse = PWM_ONTIME_START;
u16 pwm_duty = PWM_DUTY_START;   
u16 pwm_duty_old = PWM_DUTY_START;
u32 pwm_freq = 0;
u16 pwm_timebase = PWM_TBASE_START;  
u16 pwm_timebase_old = PWM_TBASE_START;
Pwm_Polarity_t pwm_polarity = Polarity_Positive;

typedef enum
{
  LCD_Update_NO_UPDATE    = (u8)0x00,
  LCD_Update_PWM_freq     = (u8)0x01,
  LCD_Update_PWM_duty     = (u8)0x02,
  LCD_Update_PWM_period   = (u8)0x04,
  LCD_Update_PWM_ontime   = (u8)0x08,
  LCD_Update_PWM_modifier = (u8)0x10
}LCD_Update_t;
LCD_Update_t LCD_Update = LCD_Update_NO_UPDATE;

TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
TIM_OCInitTypeDef TIM_OCInitStruct;

void Convert2String_PWM_freq(u32 _pwm_freq, char * _out_string);
void Convert2String_PWM_duty(u16 _pwm_duty, char * _out_string);
void Convert2String_PWM_period(u16 _pwm_period, char * _out_string);
void Convert2String_PWM_ontime(u16 _pwm_ontime, char * _out_string);

int main(void)
{
  VrefINT_CAL = *ptr_VREFINT_CAL;
  Config();
  Errors_Init();
  
  SystemCoreClockUpdate();
  
  LCD_Initialize();
  LCD_Clear();
  LCD_WriteString("Constant Current");
  
  while (1)
  {
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
    
    /* ============== LCD UPDATE CHECK ================= */
    if(LCD_Update)
    {
      switch(Current_Screen)
      {
        case Screen_ConstCurrent:
        {
          break;
        }
        case Screen_ConstPower:
        {
          break;    
        }
        case Screen_ConstResistor:
        {
          LCD_Home();
          if(LCD_Update & LCD_Update_PWM_freq)     Convert2String_PWM_freq(pwm_freq, lcd_row1);
          if(LCD_Update & LCD_Update_PWM_duty)     Convert2String_PWM_duty(pwm_duty, lcd_row1);
          if(LCD_Update & LCD_Update_PWM_period)   Convert2String_PWM_period(pwm_period, &lcd_row2[1]);
          if(LCD_Update & LCD_Update_PWM_ontime)   Convert2String_PWM_ontime(pwm_sig_pulse, &lcd_row2[1]);
          LCD_WriteString(lcd_row1);
          LCD_WriteString(lcd_row2);
          break;
        }
        default: break;
      }
      LCD_Update = LCD_Update_NO_UPDATE;
    }
    
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
      char strtmp[11];
      FLAG_250ms = FALSE;
      if(FLAG_ADC_NewData)
      {
        string_copy(lcd_row2, "I=");
        string_append(lcd_row2, string_U32ToStr((u32)((100*ADC_Conv_Tab_Avg[0])/4095), strtmp));
        string_append(lcd_row2, "% U=");
        string_append(lcd_row2, string_U32ToStr((u32)(3300*VrefINT_CAL)/(ADC_Conv_Tab_Avg[1]), strtmp));
        string_append(lcd_row2, "mV");
        LCD_WriteString(LCD_CLEAR_ROW);
        LCD_Move_Cursor(2, 0);
        LCD_WriteString(lcd_row2);
        FLAG_ADC_NewData = FALSE;
      }
    }
    /* ============== END CYCLIC ENTRIES ================= */
    
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

void Convert2String_PWM_freq(u32 _pwm_freq, char * _out_string)
{
  char l_strtmp[11];
  u32 l_mod;
  // pwm freq will be displayed on row1 leftside
  string_copy_noterm(_out_string, LCD_CLEAR_HALFROW);  // clear old information
  if(_pwm_freq < 1000)
  {
    string_copy_noterm(_out_string, string_U32ToStr(_pwm_freq, l_strtmp));
    string_append_spaceterm(_out_string, "Hz");
  }
  else if(_pwm_freq >= 1000 && _pwm_freq < 1000000)  // 1.000KHz - 999.999KHz
  {
    l_mod = (_pwm_freq)%1000;
    string_copy_noterm(_out_string, string_U32ToStr((_pwm_freq)/1000, l_strtmp));
    string_append_spaceterm(_out_string, ".");
    if(l_mod < 10) string_append_spaceterm(_out_string, "0");
    if(l_mod < 100) string_append_spaceterm(_out_string, "0");
    string_append_spaceterm(_out_string, string_U32ToStr(l_mod, l_strtmp));
    string_append_spaceterm(_out_string, "K");
  }
  else if((_pwm_freq) >= 1000000 && (_pwm_freq) < 1000000000)  // 1.000Mhz - 999.999Mhz
  {
    l_mod = ((_pwm_freq)%1000000)/1000;
    string_copy_noterm(_out_string, string_U32ToStr((_pwm_freq)/1000000, l_strtmp));
    string_append_spaceterm(_out_string, ".");
    if(l_mod < 10) string_append_spaceterm(_out_string, "0");
    if(l_mod < 100) string_append_spaceterm(_out_string, "0");
    string_append_spaceterm(_out_string, string_U32ToStr(l_mod, l_strtmp));
    string_append_spaceterm(_out_string, "M");
  }
}

void Convert2String_PWM_duty(u16 _pwm_duty, char * _out_string)
{
  char l_strtmp[11];
  u8 l_tmp = _pwm_duty/10;
  u8 l_startstr_idx;
  // pwm freq will be displayed on row1 rightside
  string_copy_noterm(&(_out_string[8]), LCD_CLEAR_DUTY);  // clear old information
  if(l_tmp < 10) 
  {
    string_copy_noterm(&(_out_string[10]), string_U32ToStr(l_tmp, l_strtmp));
    l_startstr_idx = 10;
  }
  else
  {
    string_copy_noterm(&(_out_string[9]),  string_U32ToStr(l_tmp, l_strtmp));
    l_startstr_idx = 9;
  }
  string_append_spaceterm(&(_out_string[l_startstr_idx]), ".");
  string_append_spaceterm(&(_out_string[l_startstr_idx]), string_U32ToStr(_pwm_duty%10, l_strtmp));
  string_append_spaceterm(&(_out_string[l_startstr_idx]), "%");
}

void Convert2String_PWM_period(u16 _pwm_period, char * _out_string)
{
  char   l_strtmp[11];
  u8     l_period_mul10_factor = 0;
  pwm_period_real = _pwm_period;
  if     (_pwm_period <=         4) {pwm_period_real *= 1000000000;l_period_mul10_factor = 9;} //1ns
  else if(_pwm_period <=        42) {pwm_period_real *= 100000000; l_period_mul10_factor = 8;} //10ns
  else if(_pwm_period <=       429) {pwm_period_real *= 10000000;  l_period_mul10_factor = 7;} //100ns
  else if(_pwm_period <=      4294) {pwm_period_real *= 1000000;   l_period_mul10_factor = 6;} //1us
  else if(_pwm_period <=     42949) {pwm_period_real *= 100000;    l_period_mul10_factor = 5;} //10us
  /*else if(_pwm_period <=    429496) {pwm_period_real *= 10000;     l_period_mul10_factor = 4;} //100us
  else if(_pwm_period <=   4294967) {pwm_period_real *= 1000;      l_period_mul10_factor = 3;} //1ms
  else if(_pwm_period <=  42949672) {pwm_period_real *= 100;       l_period_mul10_factor = 2;} //10ms
  else if(_pwm_period <= 429496729) {pwm_period_real *= 10;        l_period_mul10_factor = 1;} //100ms*/
  pwm_period_real = pwm_period_real / (48000000/pwm_timebase);
  
  string_copy_noterm(_out_string, LCD_CLEAR_HALFROW);  // clear old information
  
  switch(l_period_mul10_factor)
  {
    case 0: {
      string_copy_noterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "S");
      break;
    }
    case 1: {
      string_copy_noterm(_out_string, "0.");
      string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "S");
      break;
    }
    case 2: {
      string_copy_noterm(_out_string, "0.0");
      string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "S");
      break;
    }
    case 3: {
      string_copy_noterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "mS");
      break;
    }
    case 4: {
      string_copy_noterm(_out_string, "0.");
      string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "mS");
      break;
    }
    case 5: {
      string_copy_noterm(_out_string, "0.0");
      string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "mS");
      break;
    }
    case 6: {
      string_copy_noterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "uS");
      break;
    }
    case 7: {
      _out_string[0] = pwm_period_real / 10 + 48;
      _out_string[1] = '.';
      _out_string[2] = pwm_period_real % 10 + 48;
      _out_string[3] = 'u';
      _out_string[4] = 's';
      //string_copy_noterm(_out_string, "0.");
      //string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      //string_append_spaceterm(_out_string, "uS");
      break;
    }
    case 8: {
      string_copy_noterm(_out_string, "0.0");
      string_append_spaceterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "uS");
      break;
    }
    case 9: {
      string_copy_noterm(_out_string, string_U32ToStr(pwm_period_real, l_strtmp));
      string_append_spaceterm(_out_string, "nS");
      break;
    }
    default: break;
  }
}

void Convert2String_PWM_ontime(u16 _pwm_ontime, char * _out_string)
{
  char   l_strtmp[11];
  u8     l_period_mul10_factor = 0;
  pwm_ontime_real = _pwm_ontime;
  if     (_pwm_ontime <=         4) {pwm_ontime_real *= 1000000000;l_period_mul10_factor = 9;} //1ns
  else if(_pwm_ontime <=        42) {pwm_ontime_real *= 100000000; l_period_mul10_factor = 8;} //10ns
  else if(_pwm_ontime <=       429) {pwm_ontime_real *= 10000000;  l_period_mul10_factor = 7;} //100ns
  else if(_pwm_ontime <=      4294) {pwm_ontime_real *= 1000000;   l_period_mul10_factor = 6;} //1us
  else if(_pwm_ontime <=     42949) {pwm_ontime_real *= 100000;    l_period_mul10_factor = 5;} //10us
  /*else if(_pwm_ontime <=    429496) {pwm_ontime_real *= 10000;     l_period_mul10_factor = 4;} //100us
  else if(_pwm_ontime <=   4294967) {pwm_ontime_real *= 1000;      l_period_mul10_factor = 3;} //1ms
  else if(_pwm_ontime <=  42949672) {pwm_ontime_real *= 100;       l_period_mul10_factor = 2;} //10ms
  else if(_pwm_ontime <= 429496729) {pwm_ontime_real *= 10;        l_period_mul10_factor = 1;} //100ms*/
  pwm_ontime_real = pwm_ontime_real / (48000000/pwm_timebase);
  
  string_copy_noterm(&(_out_string[8]), LCD_CLEAR_HALFROW);  // clear old information
  
  switch(l_period_mul10_factor)
  {
    case 0: {
      string_copy_noterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "S");
      break;
    }
    case 1: {
      string_copy_noterm(&(_out_string[8]), "0.");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "S");
      break;
    }
    case 2: {
      string_copy_noterm(&(_out_string[8]), "0.0");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "S");
      break;
    }
    case 3: {
      string_copy_noterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "mS");
      break;
    }
    case 4: {
      string_copy_noterm(&(_out_string[8]), "0.");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "mS");
      break;
    }
    case 5: {
      string_copy_noterm(&(_out_string[8]), "0.0");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "mS");
      break;
    }
    case 6: {
      string_copy_noterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "uS");
      break;
    }
    case 7: {
      string_copy_noterm(&(_out_string[8]), "0.");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "mS");
      break;
    }
    case 8: {
      string_copy_noterm(&(_out_string[8]), "0.0");
      string_append_spaceterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "mS");
      break;
    }
    case 9: {
      string_copy_noterm(&(_out_string[8]), string_U32ToStr(pwm_ontime_real, l_strtmp));
      string_append_spaceterm(&(_out_string[8]), "nS");
      break;
    }
    default: break;
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
