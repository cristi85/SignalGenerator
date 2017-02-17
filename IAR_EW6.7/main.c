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
#include "rtms.h"
#include "stackusage.h" 
#include "ADS1112.h"

typedef enum
{
  LCD_Update_NO_UPDATE  = (u8)0x00,
  LCD_Update_Current    = (u8)0x01,
  LCD_Update_Voltage    = (u8)0x02,
  LCD_Update_Power      = (u8)0x04,
  LCD_Update_Ireq       = (u8)0x08,
  LCD_Update_Cal        = (u8)0x10,
  LCD_Update_CPU_Load   = (u8)0x20,
  LCD_Update_MaxStack   = (u8)0x40,
  LCD_Update_Error      = (u8)0x80
}LCD_Update_t;
volatile LCD_Update_t LCD_Update = LCD_Update_NO_UPDATE;

bool flag_LCD_Update_row1 = FALSE;
bool flag_LCD_Update_row2 = FALSE;
static volatile u8 step_Background_Task = 0;

void Convert2String_Current(s32 ImA);
void Convert2String_Voltage(s32 UmV);
void Convert2String_Ireq(u16 Requested_Current);
void Convert2String_Cal(u32 CurrentSenOffset);
void Convert2String_Power(s32 pPowermW);
void Convert2String_CPUload(u16 pcpuload);
void Convert2String_MaxStack(u8 pMaxStack);
void CheckSystemClock(u32* clkfreq, u8* clksrc);
void Convert2String_Errors(void);
void memcopy(u8* address, u8 data, u32 len);

#define STM_Clk_Src_HSI  (u8)0
#define STM_Clk_Src_HSE  (u8)1
extern u8 STM_Clk_Src;

/* Vref INT CAL */
const u16* ptr_VREFINT_CAL = (u16*)0x1FFFF7BA;
u16 VrefINT_CAL; 

u16 CpuLoad_old = 0;
u8  MaxStackUsage_old = 0;
u8 volatile __debug;
u8 volatile mode = 0;

/* LCD MACROS and variables */
#define LCD_CLEAR_ROW     "                "
#define LCD_CLEAR_HALFROW "        "
#define LCD_CLEAR_DUTY    "       "
static char lcd_row1[18] = "                \n";
static char lcd_row2[17] = LCD_CLEAR_ROW;

#define POWER_LIMIT   (u32)30000  /* mili Watts */
#define CURRENT_LIMIT (u32)5000   /* mili Amps */
static s32 UmV = 0; 
static s32 UmV_old = 0;
static s32 ImA = 0;
static s32 ImA_old = 0;
static s32 PowermW = 0;
static s32 PowermW_old = 0;
static u32 PowermW_modulo = 0;

#define VOLT_MEASUREMENT (u8)0
#define AMP_MEASUREMENT  (u8)1
static s16 sample = 0;   //signed sample from ADC
static u8 ADCNewSample;
static u8 ADS1112_channel = VOLT_MEASUREMENT;
const u8 AMP_Offset = 131;   //amperage measurement channel
const u8 VOLT_Offset = 40;   //voltage measurement channel
static u8 pga = 8;
static u32 temp_u32 = 0;
static bool flag_negative = FALSE;

u16 FanSpeed = 0;

/* LEM current sensor offset variables */
#define CURRENTSEN_NUMREADS (u8)32
u32 CurrentSenOffset = 0, CurrentSenOffset_acc = 0;
u8 cnt_RdCurrentSenOffset;
u8 cnt_discardADC;

u32 CPUClk = 0;
u8 CPUClkSrc = 0;

int main(void)
{
  volatile u8 status = 0xFF;
  volatile u8 cfg[8] = {1,2,3,4,5,6,7,8};
  VrefINT_CAL = *ptr_VREFINT_CAL;
  StackUsage_Init();
  Config();
  Errors_Init();
  
  while(!LCD_Initialize());
  while(!LCD_Clear());
  while(!LCD_Home());
  LCD_Update |= LCD_Update_Current;
  LCD_Update |= LCD_Update_Voltage;
  LCD_Update |= LCD_Update_Power;
  
  if(!ADS1112_Init()) {
    Errors_SetError(ERROR_ADS1112);
    LCD_Update |= LCD_Update_Error;
  }
  else {
    Errors_ResetError(ERROR_ADS1112);
  }
  
  if(!ADS1112_SetMeasurementChannel(ADS1112_channel, 1)) {
    Errors_SetError(ERROR_ADS1112);
    LCD_Update |= LCD_Update_Error;
  }
  else {
    Errors_ResetError(ERROR_ADS1112);
  }
  
  while (1)
  {
    /* ============== CYCLIC ENTRIES ================= */
    if(FLAG_10ms)
    {
      RTMS_MeasureTaskStart(RunningTask_10ms);
      FLAG_10ms = FALSE;
      //DEBUGPIN_TOGGLE;
      RTMS_MeasureTaskEnd(RunningTask_10ms);
    }
    
    /* ============== PRESS BTN INC ================= */
    if(BTN_INC_DEB_STATE == BTN_PRESSED && BTN_INC_DELAY_FLAG)
    {
      BTN_INC_DELAY_FLAG = FALSE;
    }
    
    /* ============== PRESS BTN DEC ================= */
    if(BTN_DEC_DEB_STATE == BTN_PRESSED && BTN_DEC_DELAY_FLAG)
    {
      BTN_DEC_DELAY_FLAG = FALSE;
    }
    
    /* ============== PRESS BTN MODE ================= */
    if(BTN_MODE_DEB_STATE == BTN_PRESSED && BTN_MODE_DELAY_FLAG)
    {
      BTN_MODE_DELAY_FLAG = FALSE;
    }
    if(FLAG_100ms)
    {
      RTMS_MeasureTaskStart(RunningTask_100ms);
      //DEBUGPIN_TOGGLE;
      StackUsage_CalcMaxUsage();
      /*if(StackUsage_GetMaxStackUsage() != MaxStackUsage_old) LCD_Update |= LCD_Update_MaxStack;
      MaxStackUsage_old = StackUsage_GetMaxStackUsage();*/
      FLAG_100ms = FALSE;
      
      if(!ADS1112_GetSample(&sample, &ADCNewSample)) {
        Errors_SetError(ERROR_ADS1112);
        LCD_Update |= LCD_Update_Error;
      }
      else {
        Errors_ResetError(ERROR_ADS1112);
      }
      if(!Errors_CheckError(ERROR_ADS1112))
      {
        if(ADCNewSample == 1)
        {
          //if conversion was done and new data is available
          switch(ADS1112_channel)
          {
          case VOLT_MEASUREMENT:  //if ADS1112_channel == VOLT_MEASUREMENT means that we have an amperage sample read from the ADC
            {
              ADS1112_channel = AMP_MEASUREMENT;
              if(!ADS1112_SetMeasurementChannel(ADS1112_channel, pga)) {
                Errors_SetError(ERROR_ADS1112);
                LCD_Update |= LCD_Update_Error;
              }
              else {
                Errors_ResetError(ERROR_ADS1112);
              }
              
              sample -= VOLT_Offset;
              flag_negative = FALSE;
              if(sample < 0) {
                sample = -sample;
                flag_negative = TRUE;
              }
              temp_u32 = (u32)((u32)97506 * (u32)sample);
              temp_u32 /= (u32)100000;
              
              if(flag_negative) {
                UmV = (u32)temp_u32;
                UmV = -UmV;
              }
              else {
                UmV = (u32)temp_u32;
              }
              
              if(UmV != UmV_old) LCD_Update |= LCD_Update_Voltage;
              UmV_old = UmV;
              break;
            }
          case AMP_MEASUREMENT:   //if ADS1112_channel == AMP_MEASUREMENT means that we have a voltage sample read from the ADC
            {
              sample -= AMP_Offset;
              flag_negative = FALSE;
              if(sample < 0) {
                sample = -sample;
                flag_negative = TRUE;
              }
              
              switch(pga)
              {
              case 1:
                {
                  temp_u32 = (u32)((u32)54349 * (u32)sample);
                  temp_u32 /= (u32)10000; 
                  break;
                }
              case 2:
                {
                  temp_u32 = (u32)((u32)27175 * (u32)sample);
                  temp_u32 /= (u32)10000; 
                  break;
                }
              case 4:
                {
                  temp_u32 = (u32)((u32)13587 * (u32)sample);
                  temp_u32 /= (u32)10000; 
                  break;
                }
              case 8:
                {
                  temp_u32 = (u32)((u32)67937 * (u32)sample);
                  temp_u32 /= (u32)100000; 
                  break;
                }
              }
              
              if(flag_negative) {
                ImA = (u32)temp_u32;
                ImA = -ImA;
              }
              else {
                ImA = (u32)temp_u32;
              }
              
              /* Update LCD if needed */
              if(ImA != ImA_old) LCD_Update |= LCD_Update_Current;
              ImA_old = ImA;
              
              PowermW = UmV * ImA;
              PowermW /= 1000;
              
              if(PowermW != PowermW_old) LCD_Update |= LCD_Update_Power;
              PowermW_old = PowermW;
              
              /* Adjust FAN speed */
              if(PowermW < 0) {
                PowermW_modulo = (u32)(-PowermW);
              }
              else {
                PowermW_modulo = PowermW;
              }
              if(PowermW_modulo < 5000) {
                FanSpeed = 0;
              }
              else {
                FanSpeed = (PowermW_modulo * 3838) / 30000;
              }
              if(FanSpeed > 3838) {
                FanSpeed = 3838;
              }
              TIM_SetCompare1(TIM1, FanSpeed);
              
              /* ADS1112 PGA adjustment */
              temp_u32 = (u32)((u32)(sample+AMP_Offset) * (u32)((u16)2048/pga)) / (u16)0x7FFF;
              if(temp_u32 < 256)        pga = 8;
              else if(temp_u32 < 512)   pga = 4;
              else if(temp_u32 < 1024)  pga = 2;
              else if(temp_u32 <= 2048) pga = 1;
              
              /* Start ADC measurement on the other channel */
              ADS1112_channel = VOLT_MEASUREMENT;
              if(!ADS1112_SetMeasurementChannel(ADS1112_channel, 1)) {  //Gain is always 1 for voltage measurement
                Errors_SetError(ERROR_ADS1112);
                LCD_Update |= LCD_Update_Error;
              }
              else {
                Errors_ResetError(ERROR_ADS1112);
              }
              
              break;
            }
          default: break;        
          }
        }
      }
      RTMS_MeasureTaskEnd(RunningTask_100ms);
    }
    if(FLAG_1000ms)
    {
      RTMS_CpuLoadCalculation();
      /*if(RTMS_GetCpuLoadCurrent() != CpuLoad_old) LCD_Update |= LCD_Update_CPU_Load;
      CpuLoad_old = RTMS_GetCpuLoadCurrent();*/
      
      /* Check System Clock */
      CheckSystemClock(&CPUClk, &CPUClkSrc);
      if(CPUClk != (u32)48000000) {
        Errors_SetError(ERROR_CLOCK_FREQ);
        LCD_Update |= LCD_Update_Error;
      }
      else {
        Errors_ResetError(ERROR_CLOCK_FREQ);
      }
      if(CPUClkSrc != (u8)0x08) /* CLK Src is expected to be PLL */{
        Errors_SetError(ERROR_CLOCK_SRC);
        LCD_Update |= LCD_Update_Error;
      }
      else {
        Errors_ResetError(ERROR_CLOCK_SRC);
      }
      
      FLAG_1000ms = FALSE;
    }
    
    if(FLAG_PID_500us)
    {
      RTMS_MeasureTaskStart(RunningTask_PID_500us);
      FLAG_PID_500us = FALSE;
      if(FLAG_ADC_NewData)
      {
        //DEBUGPIN_HIGH;
        /* Mark ADC data as read so new conversions can be made */
        FLAG_ADC_NewData = FALSE;
        //DEBUGPIN_LOW;
      }
      else
      {
        /* no new ADC data available in time, shoould not reach here!!! */
        __debug = __debug;
      }
      RTMS_MeasureTaskEnd(RunningTask_PID_500us);
    }

    //LCD_Update = LCD_Update_Current|LCD_Update_Voltage|LCD_Update_Ireq|LCD_Update_Power|LCD_Update_CPU_Load;
    /* BACKGROUND TASK - SHOULD NOT TAKE MORE THAN 1MS IN ONE PASS!!! */
    //DEBUGPIN_HIGH;
    RTMS_MeasureTaskStart(RunningTask_Bkg);
    /* ============== LCD UPDATE CHECK ================= */
    if(LCD_Update && LCD_UPDATE_LIMIT_FLAG)
    {
      switch(step_Background_Task)
      {
      case 0:
        {
          if(LCD_Home())
          {
            step_Background_Task = 1;
          }
          break;
        }
      case 1:
        {
          if(Errors_IsError()) {
            Convert2String_Errors();
          }
          else {
            flag_LCD_Update_row1 = FALSE;
            flag_LCD_Update_row2 = FALSE;
            /* Max Duration: 26.5us, 1 step */
            if(LCD_Update & LCD_Update_Current) {
              Convert2String_Current(ImA);
              flag_LCD_Update_row1 = TRUE;
            }
            if(LCD_Update & LCD_Update_Voltage) {
              Convert2String_Voltage(UmV);
              flag_LCD_Update_row1 = TRUE;
            }
            /*if(LCD_Update & LCD_Update_Ireq) {
            Convert2String_Ireq(Requested_Current);
            flag_LCD_Update_row2 = TRUE;
          }*/
            /*if(LCD_Update & LCD_Update_Cal) {
            Convert2String_Cal(CurrentSenOffset);
          }*/
            if(LCD_Update & LCD_Update_Power) {
              Convert2String_Power(PowermW);
              flag_LCD_Update_row2 = TRUE;
            }
            /*if(LCD_Update & LCD_Update_MaxStack) {
            Convert2String_MaxStack(StackUsage_GetMaxStackUsage());
            flag_LCD_Update_row2 = TRUE;
          }*/
            /*if(LCD_Update & LCD_Update_CPU_Load) {
            Convert2String_CPUload(RTMS_GetCpuLoadCurrent());
            flag_LCD_Update_row2 = TRUE;
          }*/
          }
          
          step_Background_Task = 2;
          break;
        }
      case 2:
        {  
          if(LCD_WriteString(lcd_row1)) {
            step_Background_Task = 3;
          } 
          break;
        }
      case 3:
        {
          if(LCD_WriteString(lcd_row2)) {
            LCD_Update = LCD_Update_NO_UPDATE;
            LCD_UPDATE_LIMIT_FLAG = FALSE;
            step_Background_Task = 0; 
          }
          break;
        }
      default:
        {
          __debug = __debug;
          break;
        }
      }
    }
    RTMS_MeasureTaskEnd(RunningTask_Bkg);
    //DEBUGPIN_LOW;
    /* ============== END LCD UPDATE CHECK ================= */
  }
}

void Convert2String_Errors()
{
  u8 row_idx = 0;
  
  string_copy_noterm(lcd_row1, LCD_CLEAR_ROW);
  string_copy_noterm(lcd_row2, LCD_CLEAR_ROW);
  
  lcd_row1[5] = 'E';
  lcd_row1[6] = 'R';
  lcd_row1[7] = 'R';
  lcd_row1[8] = 'O';
  lcd_row1[9] = 'R';
  lcd_row1[10] = ':';
  
  if(Errors_CheckError(ERROR_ADS1112)) {
    lcd_row2[row_idx++] = 'A';
    lcd_row2[row_idx++] = 'D';
    lcd_row2[row_idx++] = 'S';
    lcd_row2[row_idx++] = ',';
  }
  if(Errors_CheckError(ERROR_CLOCK_SRC)) {
    lcd_row2[row_idx++] = 'C';
    lcd_row2[row_idx++] = 'l';
    lcd_row2[row_idx++] = 'k';
    lcd_row2[row_idx++] = 'S';
    lcd_row2[row_idx++] = ',';
  }
  if(Errors_CheckError(ERROR_CLOCK_FREQ)) {
    lcd_row2[row_idx++] = 'C';
    lcd_row2[row_idx++] = 'l';
    lcd_row2[row_idx++] = 'k';
    lcd_row2[row_idx++] = 'F';
    lcd_row2[row_idx++] = ',';
  }
  lcd_row2[row_idx-1] = ' ';
}

void Convert2String_Current(s32 pImA)
{
  /* Row1 left */
  #define ROW_OFFSET_CURRENT (u8)0
  #define ROW_USED_CURRENT   lcd_row1
  u32 temp_ImA = pImA;
  u8 last_digit;
  bool flag_negative = FALSE;
  
  if(pImA < 0) {
    pImA = -pImA;
    flag_negative = TRUE;  
  }
  temp_ImA = (u32)pImA;
  
  last_digit = temp_ImA % 10;
  temp_ImA /= 10;
  if(last_digit % 10 >= 5) {
    temp_ImA++;
  }
  
  ROW_USED_CURRENT[0+ROW_OFFSET_CURRENT] = (u8)(' ');
  if(temp_ImA > 0) {
    if(flag_negative) {
      ROW_USED_CURRENT[0+ROW_OFFSET_CURRENT] = (u8)('-');
    }
  }
  
  ROW_USED_CURRENT[5+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[4+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[2+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[1+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  ROW_USED_CURRENT[3+ROW_OFFSET_CURRENT] = '.';
  ROW_USED_CURRENT[6+ROW_OFFSET_CURRENT] = 'A';
}
void Convert2String_Voltage(s32 pUmV)
{
  /* Row1 right */
#define ROW_OFFSET_VOLTAGE (u8)9
#define ROW_USED_VOLTAGE   lcd_row1
  u32 temp_UmV;
  u8 last_digit;
  bool flag_negative = FALSE;
  
  if(pUmV < 0) {
    pUmV = -pUmV;
  }
  
  temp_UmV = (u32)pUmV;
  last_digit = temp_UmV % 10;
  temp_UmV /= 10;
  if(last_digit % 10 >= 5) {
    temp_UmV++;
  }
  
  ROW_USED_VOLTAGE[0+ROW_OFFSET_VOLTAGE] = (u8)(' ');
  if(temp_UmV > 0) {
    if(flag_negative) {
      ROW_USED_VOLTAGE[0+ROW_OFFSET_VOLTAGE] = (u8)('-');
    }
  }
  
  ROW_USED_VOLTAGE[5+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[4+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[2+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[1+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  ROW_USED_VOLTAGE[3+ROW_OFFSET_VOLTAGE] = '.';
  ROW_USED_VOLTAGE[6+ROW_OFFSET_VOLTAGE] = 'V';
}

void Convert2String_Ireq(u16 pRequested_Current)
{
  /* Row2 left */
#define ROW_OFFSET_IREQ (u8)0
#define ROW_USED_IREQ   lcd_row2
  u32 temp_ReqCurrent = pRequested_Current;
  
  ROW_USED_IREQ[0+ROW_OFFSET_IREQ] = 'I';
  ROW_USED_IREQ[1+ROW_OFFSET_IREQ] = 'r';
  ROW_USED_IREQ[2+ROW_OFFSET_IREQ] = 'q';
  ROW_USED_IREQ[3+ROW_OFFSET_IREQ] = '=';
  ROW_USED_IREQ[6+ROW_OFFSET_IREQ] = (u8)(temp_ReqCurrent % 10) + 48;
  temp_ReqCurrent /= 10;
  ROW_USED_IREQ[5+ROW_OFFSET_IREQ] = (u8)(temp_ReqCurrent % 10) + 48;
  temp_ReqCurrent /= 10;
  ROW_USED_IREQ[4+ROW_OFFSET_IREQ] = (u8)(temp_ReqCurrent % 10) + 48;
}

void Convert2String_CPUload(u16 pcpuload)
{
  /* Row2 left */
#define ROW_OFFSET_CPULOAD (u8)0
#define ROW_USED_CPULOAD   lcd_row2
  u32 temp_CPUload = pcpuload;
  
  ROW_USED_CPULOAD[0+ROW_OFFSET_CPULOAD] = 'C';
  ROW_USED_CPULOAD[1+ROW_OFFSET_CPULOAD] = 'P';
  ROW_USED_CPULOAD[2+ROW_OFFSET_CPULOAD] = 'U';
  ROW_USED_CPULOAD[3+ROW_OFFSET_CPULOAD] = '=';
  ROW_USED_CPULOAD[6+ROW_OFFSET_CPULOAD] = '.';
  ROW_USED_CPULOAD[8+ROW_OFFSET_CPULOAD] = '%';
  ROW_USED_CPULOAD[7+ROW_OFFSET_CPULOAD] = (u8)(temp_CPUload % 10) + 48;
  temp_CPUload /= 10;
  ROW_USED_CPULOAD[5+ROW_OFFSET_CPULOAD] = (u8)(temp_CPUload % 10) + 48;
  temp_CPUload /= 10;
  ROW_USED_CPULOAD[4+ROW_OFFSET_CPULOAD] = (u8)(temp_CPUload % 10) + 48;
}

void Convert2String_MaxStack(u8 pMaxStack)
{
  /* Row2 left */
#define ROW_OFFSET_MAXSTACK (u8)9
#define ROW_USED_MAXSTACK   lcd_row2
  u32 temp_MaxStack = pMaxStack;
  
  ROW_USED_MAXSTACK[0+ROW_OFFSET_MAXSTACK] = 'S';
  ROW_USED_MAXSTACK[1+ROW_OFFSET_MAXSTACK] = 'T';
  ROW_USED_MAXSTACK[2+ROW_OFFSET_MAXSTACK] = '=';
  ROW_USED_MAXSTACK[5+ROW_OFFSET_MAXSTACK] = (u8)(temp_MaxStack % 10) + 48;
  temp_MaxStack /= 10;
  ROW_USED_MAXSTACK[4+ROW_OFFSET_MAXSTACK] = (u8)(temp_MaxStack % 10) + 48;
  temp_MaxStack /= 10;
  ROW_USED_MAXSTACK[3+ROW_OFFSET_MAXSTACK] = (u8)(temp_MaxStack % 10) + 48;
  ROW_USED_MAXSTACK[6+ROW_OFFSET_MAXSTACK] = '%';
}

void Convert2String_Cal(u32 CurrentSenOffset)
{
  /*  */
}
void Convert2String_Power(s32 pPowermW)
{
  /* Row2 right */
  #define ROW_OFFSET_POWER (u8)9
  #define ROW_USED_POWER   lcd_row2
  u32 temp_PowermW;
  u8 last_digit;
  
  if(pPowermW < 0) {
    pPowermW = -pPowermW;
    ROW_USED_POWER[0+ROW_OFFSET_VOLTAGE] = (u8)('-');
  }
  else {
    ROW_USED_POWER[0+ROW_OFFSET_VOLTAGE] = (u8)(' ');
  }
  temp_PowermW = (u32)pPowermW;
  last_digit = temp_PowermW % 10;
  temp_PowermW /= 10;
  if(last_digit % 10 >= 5) {
    temp_PowermW++;
  }
  
  ROW_USED_POWER[5+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[4+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[2+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[1+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  ROW_USED_POWER[3+ROW_OFFSET_POWER] = '.';
  ROW_USED_POWER[6+ROW_OFFSET_POWER] = 'W';
}

void CheckSystemClock(u32* clkfreq, u8* clksrc)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0, prediv1factor = 0;
  uint32_t l_SystemCoreClock;
  extern uint8_t AHBPrescTable[16];
  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  *clksrc = (u8)tmp;
  
  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock */
      l_SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock */
      l_SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock */
      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
      pllmull = ( pllmull >> 18) + 2;
      
      if (pllsource == 0x00)
      {
        /* HSI oscillator clock divided by 2 selected as PLL clock entry */
        l_SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {
        prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
        /* HSE oscillator clock selected as PREDIV1 clock entry */
        l_SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull; 
      }      
      break;
    default: /* HSI used as system clock */
      l_SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK clock frequency ----------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK clock frequency */
  l_SystemCoreClock >>= tmp;
  *clkfreq = l_SystemCoreClock;
}

void memcopy(u8* address, u8 data, u32 len)
{
  u32 i;
  for(i = 0; i < len; i++)
  {
    *address++ = data;
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
