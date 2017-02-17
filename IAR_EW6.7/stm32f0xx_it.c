/**
  ******************************************************************************
  * @file    TIM_Time_Base/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"
#include "stm32f0xx_it.h"
#include "config.h"
#include "rtms.h"

//ADC measurements
volatile u16 ADC_Conv_Tab_Avg[ADC_SCAN_CHANNELS];
u32 ADC_Conv_Tab_Avg_Acc[ADC_SCAN_CHANNELS];
volatile u16 ADC_Conv_Tab[ADC_SCAN_CHANNELS];
u8 adc_samp_avg_cnt = 0;
extern u16 VrefINT_CAL;
_Bool FLAG_ADC_NewData = FALSE;
//-----------------
/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup TIM_Time_Base
  * @{
  */

/* Periodic Tasks */
#define CNTVAL_10MS  (u8)5
_Bool FLAG_10ms = FALSE;
u8 cnt_flag_10ms = 0;
#define CNTVAL_100MS  (u8)50
_Bool FLAG_100ms = FALSE;
u8 cnt_flag_100ms = 0;
#define CNTVAL_250MS  (u16)125
_Bool FLAG_250ms = FALSE;
u16 cnt_flag_250ms = 0;
#define CNTVAL_500MS  (u16)250
_Bool FLAG_500ms = FALSE;
u16 cnt_flag_500ms = 0;
#define CNTVAL_1000MS (u16)500
_Bool FLAG_1000ms = FALSE;
u16 cnt_flag_1000ms = 0;

bool FLAG_PID_500us = FALSE;
/*================*/

/* Buttons debouncing and repetition delay */
#define DIG_IN_DEB_TIME   (u8)15    /* 30ms digital input debounce time */
#define BTN_REPEAT_4      (u16)125
#define BTN_REPEAT_6      (u16)83
#define BTN_REPEAT_8      (u16)62
static u16 btn_inc_delay  = BTN_DELAY_300ms;
static u16 btn_dec_delay  = BTN_DELAY_300ms;
static u16 btn_mode_delay = BTN_DELAY_300ms;
u8 btn_inc_0_cnt = 0;
u8 btn_inc_1_cnt = 0;
volatile u8 BTN_INC_DEB_STATE = BTN_DEPRESSED;
u8 btn_dec_0_cnt = 0;
u8 btn_dec_1_cnt = 0;
volatile u8 BTN_DEC_DEB_STATE = BTN_DEPRESSED;
u8 btn_mode_0_cnt = 0;
u8 btn_mode_1_cnt = 0;
volatile u8 BTN_MODE_DEB_STATE = BTN_DEPRESSED;

_Bool BTN_INC_DELAY_FLAG = FALSE;
u16 btn_inc_delay_cnt = 0;
u16 BTN_INC_press_timer = 0;
_Bool BTN_DEC_DELAY_FLAG = FALSE;
u16 btn_dec_delay_cnt = 0;
u16 BTN_DEC_press_timer = 0;
_Bool BTN_MODE_DELAY_FLAG = FALSE;
u16 btn_mode_delay_cnt = 0;
u16 BTN_MODE_press_timer = 0;
/*========================================*/

/* LCD Update limit */
bool LCD_UPDATE_LIMIT_FLAG = FALSE;
u16 lcd_update_limit_delay_cnt = 0;
#define LCD_UPDATE_LIMIT_DELAY (u8)166  /* limit LCD update to maximum once overy 166*2ms (333ms) */

u16 Tim15CAP = 0, Tim15CAP_old = 0, TIM15CAP_delta = 0;
u16 FanRPM = 0;
/* Public variables */
static u8 i;

/* External variables */
extern _Bool Timeout_istout1;
extern _Bool Timeout_istout2;
extern u16 Timeout_toutcnt1;
extern u16 Timeout_toutcnt2;
extern u16 Timeout_tout1;
extern u16 Timeout_tout2;
/*====================*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/
void IT_Init()
{
  u8 i;
  for(i = 0; i < ADC_SCAN_CHANNELS; i++)
  {
    ADC_Conv_Tab_Avg_Acc[i] = 0;
  }
  adc_samp_avg_cnt = 0;
}
void DMA1_Channel1_IRQHandler() /* every 75us - TIM15 triggered */
{
  // DEBUGPIN_TOGGLE;
  // RTMS_MeasureIntStart;
  // DMA1 Channel1 Transfer Complete interrupt handler - DMA has transferred ADC data to ADC_Conv_Tab
  // Vref, PA3, PA5
  // Vref, U, I
  /*if(!FLAG_ADC_NewData)
  {
    // Duration: 1.35us every  FLAG_ADC_NewData = TRUE
    for(i = 0; i < ADC_SCAN_CHANNELS; i++)
    {
      ADC_Conv_Tab_Avg_Acc[i] += ADC_Conv_Tab[i];
    }
    adc_samp_avg_cnt++;
    if(adc_samp_avg_cnt >= ADC_AVG_SAMP)
    {
      adc_samp_avg_cnt = 0;
      for(i = 0; i < ADC_SCAN_CHANNELS; i++)
      { 
        ADC_Conv_Tab_Avg[i] = ADC_Conv_Tab_Avg_Acc[i] / ADC_AVG_SAMP;
        ADC_Conv_Tab_Avg_Acc[i] = 0;
      }
      ADC_VOLTAGE = (u16)((ADC_VOLTAGE * VrefINT_CAL) / ADC_VREF);   //Voltage correction based on Vref
      ADC_CURRENT = (u16)((ADC_CURRENT * VrefINT_CAL) / ADC_VREF);   //Current correction based on Vref
      FLAG_ADC_NewData = TRUE;
    }
  }
  DMA1->IFCR = DMA1_IT_TC1;
  RTMS_MeasureIntEnd;*/
}
void TIM2_IRQHandler(void)
{

}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)  
{
  RTMS_MeasureIntStart;
  if(TIM3->SR & TIM_IT_Update)  // Duration 5.5us every 2ms
  {
    /* ===== CKECK PERIODIC TASKS FLAGS ===== */
    if(cnt_flag_10ms < U8_MAX) cnt_flag_10ms++;
    if(cnt_flag_10ms >= CNTVAL_10MS) 
    {
      cnt_flag_10ms = 0;
      FLAG_10ms = TRUE;
    }
    if(cnt_flag_100ms < U8_MAX) cnt_flag_100ms++;
    if(cnt_flag_100ms >= CNTVAL_100MS) 
    {
      cnt_flag_100ms = 0;
      FLAG_100ms = TRUE;
    }
    if(cnt_flag_250ms < U16_MAX) cnt_flag_250ms++;
    if(cnt_flag_250ms >= CNTVAL_250MS) 
    {
      cnt_flag_250ms = 0;
      FLAG_250ms = TRUE;
    }
    if(cnt_flag_500ms < U16_MAX) cnt_flag_500ms++;
    if(cnt_flag_500ms >= CNTVAL_500MS) 
    {
      cnt_flag_500ms = 0;
      FLAG_500ms = TRUE;
    }
    if(cnt_flag_1000ms < U16_MAX) cnt_flag_1000ms++;
    if(cnt_flag_1000ms >= CNTVAL_1000MS) 
    {
      cnt_flag_1000ms = 0;
      FLAG_1000ms = TRUE;
    }
    /* ===== CHECK TIMEOUTS ===== */
    if(!Timeout_istout1)
    {
      Timeout_toutcnt1++;
      if(Timeout_toutcnt1 >= Timeout_tout1) Timeout_istout1 = TRUE;
    }
    if(!Timeout_istout2)
    {
      Timeout_toutcnt2++;
      if(Timeout_toutcnt2 >= Timeout_tout2) Timeout_istout2 = TRUE;
    }
    /* ========== DEBOUNCE INPUTS ========== 2MS */
    /* Debounce BTN INC */
    if(!BTN_FREQINC_STATE)
    {
      if(btn_mode_0_cnt < U8_MAX) btn_mode_0_cnt++;
      btn_mode_1_cnt = 0;
      if(btn_mode_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_MODE_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_mode_1_cnt < U8_MAX) btn_mode_1_cnt++;
      btn_mode_0_cnt = 0;
      if(btn_mode_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_MODE_DEB_STATE = BTN_DEPRESSED;
        BTN_MODE_press_timer = 0;
        btn_mode_delay = BTN_DELAY_300ms;
      }
    }
    /* Debounce BTN DEC */
    if(!BTN_FREQDEC_STATE)
    {
      if(btn_dec_0_cnt < U8_MAX) btn_dec_0_cnt++;
      btn_dec_1_cnt = 0;
      if(btn_dec_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_DEC_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_dec_1_cnt < U8_MAX) btn_dec_1_cnt++;
      btn_dec_0_cnt = 0;
      if(btn_dec_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_DEC_DEB_STATE = BTN_DEPRESSED;
        BTN_DEC_press_timer = 0;
        btn_dec_delay = BTN_DELAY_300ms;
      }
    }
    /* Debounce BTN MODE */
    if(!BTN_FREQDUTY_STATE)
    {
      if(btn_inc_0_cnt < U8_MAX) btn_inc_0_cnt++;
      btn_inc_1_cnt = 0;
      if(btn_inc_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_INC_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_inc_1_cnt < U8_MAX) btn_inc_1_cnt++;
      btn_inc_0_cnt = 0;
      if(btn_inc_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_INC_DEB_STATE = BTN_DEPRESSED;
        BTN_INC_press_timer = 0;
	btn_inc_delay = BTN_DELAY_300ms;
      }
    }
    /* Record button press time and adjust delay */
    if(BTN_INC_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_INC_press_timer < U16_MAX) BTN_INC_press_timer++;
      if(BTN_INC_press_timer > BTN_DELAY_1000ms && BTN_INC_press_timer < BTN_DELAY_2500ms)       btn_inc_delay = BTN_REPEAT_4;
      else if(BTN_INC_press_timer >= BTN_DELAY_2500ms && BTN_INC_press_timer < BTN_DELAY_5000ms) btn_inc_delay = BTN_REPEAT_6;
      else if(BTN_INC_press_timer >= BTN_DELAY_5000ms)                                           btn_inc_delay = BTN_REPEAT_8;
    }
    if(BTN_DEC_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_DEC_press_timer < U16_MAX) BTN_DEC_press_timer++;
      if(BTN_DEC_press_timer > BTN_DELAY_1000ms && BTN_DEC_press_timer < BTN_DELAY_2500ms)       btn_dec_delay = BTN_REPEAT_4;
      else if(BTN_DEC_press_timer >= BTN_DELAY_2500ms && BTN_DEC_press_timer < BTN_DELAY_5000ms) btn_dec_delay = BTN_REPEAT_6;
      else if(BTN_DEC_press_timer >= BTN_DELAY_5000ms)                                           btn_dec_delay = BTN_REPEAT_8;
    }
    if(BTN_MODE_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_MODE_press_timer < U16_MAX) BTN_MODE_press_timer++;
    }
    /* Process button repetition rate delays */
    if(!BTN_INC_DELAY_FLAG)
    {
      btn_inc_delay_cnt++;
      if(btn_inc_delay_cnt >= btn_inc_delay)
      {
        btn_inc_delay_cnt = 0;
        BTN_INC_DELAY_FLAG = TRUE;
      }
    }
    if(!BTN_DEC_DELAY_FLAG)
    {
      btn_dec_delay_cnt++;
      if(btn_dec_delay_cnt >= btn_dec_delay)
      {
        btn_dec_delay_cnt = 0;
        BTN_DEC_DELAY_FLAG = TRUE;
      }
    }
    if(!BTN_MODE_DELAY_FLAG)
    {
      btn_mode_delay_cnt++;
      if(btn_mode_delay_cnt >= btn_mode_delay)
      {
        btn_mode_delay_cnt = 0;
        BTN_MODE_DELAY_FLAG = TRUE;
      }
    }
    /* ======================================= */
    /* LCD update limit */
    if(lcd_update_limit_delay_cnt < U16_MAX) lcd_update_limit_delay_cnt++;
    if(lcd_update_limit_delay_cnt >= LCD_UPDATE_LIMIT_DELAY)
    {
      lcd_update_limit_delay_cnt = 0;
      LCD_UPDATE_LIMIT_FLAG = TRUE;
    }
    /* Clear the IT pending Bit */
    TIM3->SR = (u16)~TIM_IT_Update;
  }
  RTMS_MeasureIntEnd;
}

void TIM17_IRQHandler(void)  
{
  //DEBUGPIN_TOGGLE;
  RTMS_MeasureIntStart;
  if(TIM17->SR & TIM_IT_Update)
  {
    FLAG_PID_500us = TRUE;
    
    /* Clear the IT pending Bit */
    TIM17->SR = (u16)~TIM_IT_Update;
  }
  RTMS_MeasureIntEnd;
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_IRQHandler(void)
{
  /* Check on I2C1 SMBALERT flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_ALERT))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_ALERT);
  }
  /* Check on I2C1 Time out flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_TIMEOUT))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_TIMEOUT);
  }
  /* Check on I2C1 Arbitration Lost flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_ARLO))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_ARLO);
  }   
  /* Check on I2C1 PEC error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_PECERR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_PECERR);
  } 
  /* Check on I2C2 Overrun/Underrun error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_OVR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_OVR);
  } 
  /* Check on I2C1 Acknowledge failure error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_NACKF))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_NACKF);
  }
  /* Check on I2C1 Bus error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_BERR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_BERR);
  }   
}

void TIM15_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM15, TIM_IT_CC2) == SET)
  {
    TIM_ClearITPendingBit(TIM15, TIM_IT_CC2);
    Tim15CAP = TIM_GetCapture2(TIM15);
    TIM15CAP_delta = Tim15CAP - Tim15CAP_old;
    FanRPM = (u32)60000000 / TIM15CAP_delta;
    Tim15CAP_old = Tim15CAP;
  }
}

void USART1_IRQHandler(void)
{
  while(1);
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
