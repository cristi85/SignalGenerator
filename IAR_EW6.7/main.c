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
  LCD_Update_MaxStack   = (u8)0x40
}LCD_Update_t;
volatile LCD_Update_t LCD_Update = LCD_Update_NO_UPDATE;

bool flag_LCD_Update_row1 = FALSE;
bool flag_LCD_Update_row2 = FALSE;
static u8 step_Background_Task = 0;

void Convert2String_Current(u32 ImA);
void Convert2String_Voltage(u32 UmV);
void Convert2String_Ireq(u16 Requested_Current);
void Convert2String_Cal(u32 CurrentSenOffset);
void Convert2String_Power(u32 PowermW);
void Convert2String_CPUload(u16 pcpuload);
void Convert2String_MaxStack(u8 pMaxStack);

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
static u32 UmV = 0; 
static u32 UmV_old = 0;
static u32 ImA = 0;
static u32 ImA_old = 0;
static u32 PowermW = 0;
static u32 PowermW_old = 0;
static u16 Requested_Current = 500, Requested_Current_old = 0;
bool FLAG_Power_limit = FALSE;
bool FLAG_Current_limit = FALSE;

#define AMP_MEASUREMENT  (u8)0
#define VOLT_MEASUREMENT (u8)1
static s16 sample = 0;   //signed sample from ADC
static u8 ADCNewSample;
static u8 ADS1112_channel = 0;
const u8 CH1_Offset = 86;   //amperage measurement channel
const u8 CH2_Offset = 71;   //voltage measurement channel
static u8 PGA = 1, PGAold = 1;

/* LEM current sensor offset variables */
#define CURRENTSEN_NUMREADS (u8)32
u32 CurrentSenOffset = 0, CurrentSenOffset_acc = 0;
u8 cnt_RdCurrentSenOffset;
u8 cnt_discardADC;

int main(void)
{
  volatile u8 status = 0xFF;
  volatile u8 cfg[8] = {1,2,3,4,5,6,7,8};
  VrefINT_CAL = *ptr_VREFINT_CAL;
  StackUsage_Init();
  Config();
  Errors_Init();
  
  SystemCoreClockUpdate();
  
  while(!LCD_Initialize());
  while(!LCD_Clear());
  while(!LCD_Home());
  LCD_Update |= LCD_Update_Current;
  LCD_Update |= LCD_Update_Voltage;
  
  ADS1112_Init();
  ADS1112_TriggerConversion();
  if(Errors_CheckError(ERROR_ADS1112))
  { 
    while(!LCD_Clear());
    while(!LCD_Home());
    while(!LCD_WriteString("ADS1112 Error!"));
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
      if(Requested_Current < U16_MAX) Requested_Current += 500; //500mA steps
      if(Requested_Current != Requested_Current_old) LCD_Update |= LCD_Update_Ireq;
      Requested_Current_old = Requested_Current;
    }
    
    /* ============== PRESS BTN DEC ================= */
    if(BTN_DEC_DEB_STATE == BTN_PRESSED && BTN_DEC_DELAY_FLAG)
    {
      BTN_DEC_DELAY_FLAG = FALSE;
      if(Requested_Current >= 500) Requested_Current -= 500;
      if(Requested_Current != Requested_Current_old) LCD_Update |= LCD_Update_Ireq;
      Requested_Current_old = Requested_Current;
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
      if(StackUsage_GetMaxStackUsage() != MaxStackUsage_old) LCD_Update |= LCD_Update_MaxStack;
      MaxStackUsage_old = StackUsage_GetMaxStackUsage();
      FLAG_100ms = FALSE;
      
      ADS1112_GetSample(&sample, &ADCNewSample);
      if(!Errors_CheckError(ERROR_ADS1112))
      {
        if(ADCNewSample == 1)
        {
          //change ADC conversion channel
          if(ADS1112_channel == AMP_MEASUREMENT)
          {
            ADS1112_channel = VOLT_MEASUREMENT;
            ADS1112_SetMeasurementChannel(ADS1112_channel, 1);  //Gain is always 1 for voltage measurement
          }
          else if(ADS1112_channel == VOLT_MEASUREMENT)
          {
            ADS1112_channel = AMP_MEASUREMENT;
            ADS1112_SetMeasurementChannel(ADS1112_channel, PGA);
          }
          //if conversion was done and new data is available
          switch(ADS1112_channel)
          {
          case VOLT_MEASUREMENT:  //if ADS1112_channel == VOLT_MEASUREMENT means that we have an amperage sample read from the ADC
            {
              sample -= CH1_Offset;    //offset compensation
              //Calculate amperage value and display strings based on PGA value
              //Calc_amp_disp_str();
              
              //Autoscale amperage input based on shunt voltage
              //Autoscale_amp_input();
              
              break;
            }
          case AMP_MEASUREMENT:   //if ADS1112_channel == AMP_MEASUREMENT means that we have a voltage sample read from the ADC
            {
              sample -= CH2_Offset;    //offset compensation
              //Calculate and display measured voltage
              //Calc_volt_disp_str();
              
              //Calculate and display dissipated power
              //Calc_pwr_disp_str();
              
              //Update display
              //(Runtime=12.73ms)
              //LCD_Home();
              //LCD_WriteString(lcd_row1);
              //LCD_WriteString(lcd_row2);
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
      if(RTMS_GetCpuLoadCurrent() != CpuLoad_old) LCD_Update |= LCD_Update_CPU_Load;
      CpuLoad_old = RTMS_GetCpuLoadCurrent();
      FLAG_1000ms = FALSE;
    }
    
    if(FLAG_PID_500us)
    {
      RTMS_MeasureTaskStart(RunningTask_PID_500us);
      FLAG_PID_500us = FALSE;
      if(FLAG_ADC_NewData)
      {
        //DEBUGPIN_HIGH;
        /* Read ADC conversion results */
        ImA = /*ADC_CURRENT*/0;
        UmV = /*ADC_VOLTAGE*/0;
        /* Mark ADC data as read so new conversions can be made */
        FLAG_ADC_NewData = FALSE;
        
        /*if(ImA < CurrentSenOffset) { 
          ImA = 0;
        }
        else {
          ImA -= CurrentSenOffset;
        }*/
        /* LEM HLSR 40-P/SP33 (40A) -> 11.5mV/A ; ADC LSB (12bit) -> 70.058mA */
        /*if(ImA != 0) {
          ImA *= 70058;
          ImA /= 1000;
        }
        if(ImA != ImA_old) LCD_Update |= LCD_Update_Current;
        ImA_old = ImA;
        */
        /* R1 = 4k7, R2 = 47k k=0.09042649727767695, z=11.058705469141996989463 */
        /* UmV: 0 ... 4095  */
        /* UmV: 0 ... 36.493728V */
        /*if(UmV != 0) {
          UmV *= 891178;
          UmV /= 100000;
        }
        if(UmV != UmV_old) LCD_Update |= LCD_Update_Voltage;
        UmV_old = UmV;
        
        PowermW = ImA * UmV;
        PowermW /= 1000;
        if(PowermW != PowermW_old) LCD_Update |= LCD_Update_Power;
        PowermW_old = PowermW;
        */
        /*if(mode == 0) {
          DAC_SetChannel1Data(DAC_Align_12b_R, PID_Update2(Requested_Current - ImA));
        }
        else {
          DAC_SetChannel1Data(DAC_Align_12b_R, (u16)(ImA_test)); 
        }*/
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
            step_Background_Task++;
          }
          break;
        }
      case 1:
        {
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
          if(LCD_Update & LCD_Update_Cal) {
            Convert2String_Cal(CurrentSenOffset);
          }
          /*if(LCD_Update & LCD_Update_Power) {
            Convert2String_Power(PowermW);
            flag_LCD_Update_row2 = TRUE;
          }*/
          if(LCD_Update & LCD_Update_MaxStack) {
            Convert2String_MaxStack(StackUsage_GetMaxStackUsage());
            flag_LCD_Update_row2 = TRUE;
          }
          if(LCD_Update & LCD_Update_CPU_Load) {
            Convert2String_CPUload(RTMS_GetCpuLoadCurrent());
            flag_LCD_Update_row2 = TRUE;
          }
          step_Background_Task++;
          break;
        }
      case 2:
        {  
          if(LCD_WriteString(lcd_row1)) {
            step_Background_Task++;
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
      default: break;
      }
    }
    RTMS_MeasureTaskEnd(RunningTask_Bkg);
    //DEBUGPIN_LOW;
    /* ============== END LCD UPDATE CHECK ================= */
  }
}

void Convert2String_Current(u32 pImA)
{
  /* Row1 left */
  #define ROW_OFFSET_CURRENT (u8)0
  #define ROW_USED_CURRENT   lcd_row1
  u32 temp_ImA = pImA;
  u8 last_digit;
  
  last_digit = temp_ImA % 10;
  temp_ImA /= 10;
  if(last_digit % 10 >= 5) {
    temp_ImA++;
  }
  
  ROW_USED_CURRENT[4+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[3+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[1+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  temp_ImA /= 10;
  ROW_USED_CURRENT[0+ROW_OFFSET_CURRENT] = (u8)(temp_ImA % 10) + 48;
  ROW_USED_CURRENT[2+ROW_OFFSET_CURRENT] = '.';
  ROW_USED_CURRENT[6+ROW_OFFSET_CURRENT] = 'A';
}
void Convert2String_Voltage(u32 pUmV)
{
  /* Row1 right */
#define ROW_OFFSET_VOLTAGE (u8)9
#define ROW_USED_VOLTAGE   lcd_row1
  u32 temp_UmV = pUmV;
  u8 last_digit;
  
  last_digit = temp_UmV % 10;
  temp_UmV /= 10;
  if(last_digit % 10 >= 5) {
    temp_UmV++;
  }
  
  ROW_USED_VOLTAGE[4+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[3+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[1+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  temp_UmV /= 10;
  ROW_USED_VOLTAGE[0+ROW_OFFSET_VOLTAGE] = (u8)(temp_UmV % 10) + 48;
  ROW_USED_VOLTAGE[2+ROW_OFFSET_VOLTAGE] = '.';
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
void Convert2String_Power(u32 PowermW)
{
  /* Row2 right */
  #define ROW_OFFSET_POWER (u8)9
  #define ROW_USED_POWER   lcd_row2
  u32 temp_PowermW = PowermW;
  u8 last_digit;
  
  last_digit = temp_PowermW % 10;
  temp_PowermW /= 10;
  if(last_digit % 10 >= 5) {
    temp_PowermW++;
  }
  
  ROW_USED_POWER[4+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[3+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[1+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  temp_PowermW /= 10;
  ROW_USED_POWER[0+ROW_OFFSET_POWER] = (u8)(temp_PowermW % 10) + 48;
  ROW_USED_POWER[2+ROW_OFFSET_POWER] = '.';
  ROW_USED_POWER[6+ROW_OFFSET_POWER] = 'W';
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
