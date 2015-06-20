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

//ADC measurements
extern volatile u16 ADC_Conv_Tab_Avg[ADC_Scan_Channels];  /* 0 - PA5, 1 - Vref */
u16 ADC_Conv_Tab_Avg_Acc[ADC_Scan_Channels];
volatile u16 ADC_Conv_Tab[ADC_Scan_Channels];             /* 0 - PA5, 1 - Vref */
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
#define CNTVAL_250MS  (u16)125
_Bool FLAG_250ms = FALSE;
u16 cnt_flag_250ms = 0;
#define CNTVAL_500MS  (u16)250
_Bool FLAG_500ms = FALSE;
u16 cnt_flag_500ms = 0;
#define CNTVAL_1000MS (u16)500
_Bool FLAG_1000ms = FALSE;
u16 cnt_flag_1000ms = 0;
/*================*/

/* Buttons debouncing and repetition delay */
#define DIG_IN_DEB_TIME   (u8)15    /* 30ms digital input debounce time */
#define BTN_DELAY_300ms   (u8)150   /* 300ms */
#define BTN_DELAY_1000ms  (u16)500  /* 1s */
#define BTN_DELAY_2500ms  (u16)1250 /* 2.5s */
#define BTN_DELAY_5000ms  (u16)2500 /* 5s */
static u16 btn_freqinc_delay = BTN_DELAY_300ms;
static u16 btn_freqdec_delay = BTN_DELAY_300ms;
static u16 btn_freqduty_delay = BTN_DELAY_300ms;
static u16 btn_chgwave_delay = BTN_DELAY_300ms;
u8 btn_freqinc_0_cnt = 0;
u8 btn_freqinc_1_cnt = 0;
volatile u8 BTN_FREQINC_DEB_STATE = BTN_DEPRESSED;
u8 btn_freqdec_0_cnt = 0;
u8 btn_freqdec_1_cnt = 0;
volatile u8 BTN_FREQDEC_DEB_STATE = BTN_DEPRESSED;
u8 btn_freqduty_0_cnt = 0;
u8 btn_freqduty_1_cnt = 0;
volatile u8 BTN_FREQDUTY_DEB_STATE = BTN_DEPRESSED;
u8 btn_chgwave_0_cnt = 0;
u8 btn_chgwave_1_cnt = 0;
volatile u8 BTN_CHGWAVE_DEB_STATE = BTN_DEPRESSED;

_Bool BTN_FREQINC_DELAY_FLAG = FALSE;
u16 btn_freqinc_delay_cnt = 0;
u16 BTN_FREQINC_press_timer = 0;
_Bool BTN_FREQDEC_DELAY_FLAG = FALSE;
u16 btn_freqdec_delay_cnt = 0;
u16 BTN_FREQDEC_press_timer = 0;
_Bool BTN_FREQDUTY_DELAY_FLAG = FALSE;
u16 btn_freqduty_delay_cnt = 0;
u16 BTN_FREQDUTY_press_timer = 0;
_Bool BTN_CHGWAVE_DELAY_FLAG = FALSE;
u16 btn_chgwave_delay_cnt = 0;
u16 BTN_CHGWAVE_press_timer = 0;
/*========================================*/

// ===== UART Receive =====
#define UART_TIMEOUT (u8)50   /* 100ms */
UART_CMD_T UART_CMD;
u8 UARTrcvbuffidx = 0;
u8 UARTdatachksum;
u8 UARTcmd;
u8 UARTdatalen;
u8 UARTtimeoutcnt = 0;
_Bool FLAG_UART_timeout_started = FALSE;
typedef enum {
          UART_RCV_CMD          = 0,
          UART_RCV_CMD_NEG      = 1,
          UART_RCV_DATALEN      = 2,
          UART_RCV_DATALEN_NEG  = 3,
          UART_RCV_DATA         = 4,
          UART_RCV_CHKSUM       = 5
} UART_RcvState_t;
UART_RcvState_t UART_rcv_state = UART_RCV_CMD;
_Bool FLAG_UART_cmd_rcv = FALSE;
// ===== END UART Receive =====

/* Public variables */
volatile _Bool FLAG_UARTcmdRcv = FALSE;

/* External variables */
extern _Bool Timeout_istout1;
extern _Bool Timeout_istout2;
extern u16 Timeout_toutcnt1;
extern u16 Timeout_toutcnt2;
extern u16 Timeout_tout1;
extern u16 Timeout_tout2;
extern const volatile struct CalibData CAL;
/*====================*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/
void IT_Init()
{
  u8 i;
  for(i = 0; i < ADC_Scan_Channels; i++)
  {
    ADC_Conv_Tab_Avg_Acc[i] = 0;
  }
  adc_samp_avg_cnt = 0;
}
void DMA1_Channel1_IRQHandler()
{
  u8 i;
  /* DMA1 Channel1 Transfer Complete interrupt handler - DMA has transferred ADC data to ADC_Conv_Tab */
  /* 0 - PA5, 1 - Vref */
  if(!FLAG_ADC_NewData)
  {
    for(i = 0; i < ADC_Scan_Channels; i++)
    {
      ADC_Conv_Tab_Avg_Acc[i] += ADC_Conv_Tab[i];
    }
    adc_samp_avg_cnt++;
    if(adc_samp_avg_cnt == CAL.ADC_Samp_Avg)
    {
      adc_samp_avg_cnt = 0;
      for(i = 0; i < ADC_Scan_Channels; i++)
      { 
        ADC_Conv_Tab_Avg[i] = ADC_Conv_Tab_Avg_Acc[i] / CAL.ADC_Samp_Avg;
        ADC_Conv_Tab_Avg_Acc[i] = 0;
      }
      ADC_Conv_Tab_Avg[0] = (u16)((ADC_Conv_Tab_Avg[0] * VrefINT_CAL) / ADC_Conv_Tab_Avg[1]);
      FLAG_ADC_NewData = TRUE;
    }
  }
  DMA_ClearITPendingBit(DMA1_IT_TC1);
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
  if(TIM_GetITStatus(TIM3, TIM_IT_Update))  //2ms
  {
    /* ===== CKECK PERIODIC TASKS FLAGS ===== */
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
    /* Debounce BTN FREQINC */
    if(!BTN_FREQINC_STATE)
    {
      if(btn_freqinc_0_cnt < U8_MAX) btn_freqinc_0_cnt++;
      btn_freqinc_1_cnt = 0;
      if(btn_freqinc_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQINC_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_freqinc_1_cnt < U8_MAX) btn_freqinc_1_cnt++;
      btn_freqinc_0_cnt = 0;
      if(btn_freqinc_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQINC_DEB_STATE = BTN_DEPRESSED;
        BTN_FREQINC_press_timer = 0;
        btn_freqinc_delay = BTN_DELAY_300ms;
      }
    }
    /* Debounce BTN FREQDEC */
    if(!BTN_FREQDEC_STATE)
    {
      if(btn_freqdec_0_cnt < U8_MAX) btn_freqdec_0_cnt++;
      btn_freqdec_1_cnt = 0;
      if(btn_freqdec_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQDEC_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_freqdec_1_cnt < U8_MAX) btn_freqdec_1_cnt++;
      btn_freqdec_0_cnt = 0;
      if(btn_freqdec_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQDEC_DEB_STATE = BTN_DEPRESSED;
        BTN_FREQDEC_press_timer = 0;
        btn_freqdec_delay = BTN_DELAY_300ms;
      }
    }
    /* Debounce BTN FREQDUTY */
    if(!BTN_FREQDUTY_STATE)
    {
      if(btn_freqduty_0_cnt < U8_MAX) btn_freqduty_0_cnt++;
      btn_freqduty_1_cnt = 0;
      if(btn_freqduty_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQDUTY_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_freqduty_1_cnt < U8_MAX) btn_freqduty_1_cnt++;
      btn_freqduty_0_cnt = 0;
      if(btn_freqduty_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_FREQDUTY_DEB_STATE = BTN_DEPRESSED;
        BTN_FREQDUTY_press_timer = 0;
	btn_freqduty_delay = BTN_DELAY_300ms;
      }
    }
    /* Debounce BTN CHGWAVE */
    if(!BTN_CHGWAVE_STATE)
    {
      if(btn_chgwave_0_cnt < U8_MAX) btn_chgwave_0_cnt++;
      btn_chgwave_1_cnt = 0;
      if(btn_chgwave_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_CHGWAVE_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn_chgwave_1_cnt < U8_MAX) btn_chgwave_1_cnt++;
      btn_chgwave_0_cnt = 0;
      if(btn_chgwave_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN_CHGWAVE_DEB_STATE = BTN_DEPRESSED;
        BTN_CHGWAVE_press_timer = 0;
	btn_chgwave_delay = BTN_DELAY_300ms;
      }
    }
    /* Record button press time and adjust delay */
    if(BTN_FREQINC_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_FREQINC_press_timer < U16_MAX) BTN_FREQINC_press_timer++;
      if(BTN_FREQINC_press_timer > BTN_DELAY_1000ms &&\
         BTN_FREQINC_press_timer < BTN_DELAY_2500ms)          btn_freqinc_delay = 50;  //10times/s
      else if(BTN_FREQINC_press_timer >= BTN_DELAY_2500ms &&\
         BTN_FREQINC_press_timer < BTN_DELAY_5000ms)          btn_freqinc_delay = 25;  //20 times/s
      else if(BTN_FREQINC_press_timer >= BTN_DELAY_5000ms)    btn_freqinc_delay = 10;  //100 times/s
    }
    if(BTN_FREQDEC_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_FREQDEC_press_timer < U16_MAX) BTN_FREQDEC_press_timer++;
      if(BTN_FREQDEC_press_timer > BTN_DELAY_1000ms && \
         BTN_FREQDEC_press_timer < BTN_DELAY_2500ms)       btn_freqdec_delay = 50;  //10times/s
      else if(BTN_FREQDEC_press_timer >= BTN_DELAY_2500ms && \
         BTN_FREQDEC_press_timer < BTN_DELAY_5000ms)       btn_freqdec_delay = 25;  //20 times/s
      else if(BTN_FREQDEC_press_timer >= BTN_DELAY_5000ms) btn_freqdec_delay = 10; //100 times/s
    }
    if(BTN_FREQDUTY_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_FREQDUTY_press_timer < U16_MAX) BTN_FREQDUTY_press_timer++;
    }
    if(BTN_CHGWAVE_DEB_STATE == BTN_PRESSED)
    {
      if(BTN_CHGWAVE_press_timer < U16_MAX) BTN_CHGWAVE_press_timer++;
    }
    /* Process button repetition rate delays */
    if(!BTN_FREQINC_DELAY_FLAG)
    {
      btn_freqinc_delay_cnt++;
      if(btn_freqinc_delay_cnt >= btn_freqinc_delay)
      {
        btn_freqinc_delay_cnt = 0;
        BTN_FREQINC_DELAY_FLAG = TRUE;
      }
    }
    if(!BTN_FREQDEC_DELAY_FLAG)
    {
      btn_freqdec_delay_cnt++;
      if(btn_freqdec_delay_cnt >= btn_freqdec_delay)
      {
        btn_freqdec_delay_cnt = 0;
        BTN_FREQDEC_DELAY_FLAG = TRUE;
      }
    }
    if(!BTN_FREQDUTY_DELAY_FLAG)
    {
      btn_freqduty_delay_cnt++;
      if(btn_freqduty_delay_cnt >= btn_freqduty_delay)
      {
        btn_freqduty_delay_cnt = 0;
        BTN_FREQDUTY_DELAY_FLAG = TRUE;
      }
    }
    if(!BTN_CHGWAVE_DELAY_FLAG)
    {
      btn_chgwave_delay_cnt++;
      if(btn_chgwave_delay_cnt >= btn_chgwave_delay)
      {
        btn_chgwave_delay_cnt = 0;
        BTN_CHGWAVE_DELAY_FLAG = TRUE;
      }
    }
    /* ======================================= */
    // ====== CHECK UART COMMUNICATION TIMEOUT ======
    if(UARTtimeoutcnt < U8_MAX) UARTtimeoutcnt++;
    if(UARTtimeoutcnt >= UART_TIMEOUT)
    {
      UART_rcv_state = UART_RCV_CMD;
    }
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);        // clear TIM1 CC2 interrupt flag
  }
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    u8 uart_rx = USART_ReceiveData(USART1);
    UARTtimeoutcnt = 0;     //reset timeout counter
    if(!FLAG_UART_cmd_rcv)  //if previously received command was processed by application, store in buffer, else discard sent data
    {
      switch(UART_rcv_state)
      {
        case UART_RCV_CMD:
        {
          UART_CMD.CMD = uart_rx;
          UART_rcv_state = UART_RCV_CMD_NEG;
          break;
        }
        case UART_RCV_CMD_NEG:
        {
          uart_rx = (u8)(~uart_rx);
          if(UART_CMD.CMD == uart_rx)
          {
            UART_rcv_state = UART_RCV_DATALEN;
            UARTrcvbuffidx = 0;
          }
          else
          {
            UART_rcv_state = UART_RCV_CMD;
            //acknowledge command validation failed
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x1F);
          }
          break;
        }
        case UART_RCV_DATALEN:
        {
          UART_CMD.DATAlen = uart_rx;
          UART_rcv_state = UART_RCV_DATALEN_NEG;
          break;
        }
        case UART_RCV_DATALEN_NEG:
        {
          uart_rx = (u8)(~uart_rx);
          if(UART_CMD.DATAlen == uart_rx)
          {
            if(UART_CMD.DATAlen > UARTBUFFSIZE)
            {
              UART_rcv_state = UART_RCV_CMD;
              //acknowledge data length too long failed
              while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
              USART_SendData(USART1, 0x3F);
            }
            else if(UART_CMD.DATAlen == 0)
            {
              FLAG_UART_cmd_rcv = TRUE;
              //send message received correct acknowledge
              while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
              USART_SendData(USART1, 0x10);
            }
            else UART_rcv_state = UART_RCV_DATA;
          }
          else
          {
            UART_rcv_state = UART_RCV_CMD;
            //acknowledge data length validation failed
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x2F);
          }
          break;
        }
        case UART_RCV_DATA:
        {
          if(UARTrcvbuffidx < UART_CMD.DATAlen)
          {
            UART_CMD.DATA[UARTrcvbuffidx++] = uart_rx;
            if(UARTrcvbuffidx == UART_CMD.DATAlen)
            {
              UART_rcv_state = UART_RCV_CHKSUM;
            }
          }
          break;
        }
        case UART_RCV_CHKSUM:
        {
          u8 calcchksum = 0, i;
          UARTdatachksum = uart_rx;
          //calculate received data checksum
          for(i = 0; i < UARTrcvbuffidx; i++)
          {
            calcchksum += UART_CMD.DATA[i];
          }
          calcchksum = (u8)(~calcchksum);
          //compare with received checksum
          if(calcchksum == UARTdatachksum)
          {
            FLAG_UART_cmd_rcv = TRUE;
            //send message received correct acknowledge
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x10);
          }
          else
          {
            //send message received ok but checksum failed
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x4F);
          }
          UART_rcv_state = UART_RCV_CMD;
        
          break;
        }
        default : break;
      }
    }
    else
    {
      // if previously received command was not processed by application ,discard data and send information byte
      while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
      USART_SendData(USART1, 0x5F);
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
