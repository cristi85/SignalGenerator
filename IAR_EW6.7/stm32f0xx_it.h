/**
  ******************************************************************************
  * @file    TIM_Time_Base/stm32f0xx_it.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   This file contains the headers of the interrupt handlers.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0XX_IT_H
#define __STM32F0XX_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"

#define UARTCMDLEN        (u8)32
#define BTN_DEPRESSED     (u8)0
#define BTN_PRESSED       (u8)1
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern volatile u8 BTN_FREQINC_DEB_STATE;
extern volatile u8 BTN_FREQDEC_DEB_STATE;
extern volatile u8 BTN_FREQDUTY_DEB_STATE;
extern volatile u8 BTN_CHGWAVE_DEB_STATE;
extern _Bool BTN_FREQINC_DELAY_FLAG;
extern _Bool BTN_FREQDEC_DELAY_FLAG;
extern _Bool BTN_FREQDUTY_DELAY_FLAG;
extern _Bool BTN_CHGWAVE_DELAY_FLAG;
extern u16 BTN_FREQINC_press_timer;
extern u16 BTN_FREQDEC_press_timer;
extern u16 BTN_FREQDUTY_press_timer;
extern u16 BTN_CHGWAVE_press_timer;

extern _Bool FLAG_250ms;
extern _Bool FLAG_500ms;
extern _Bool FLAG_1000ms;
extern UART_CMD_T UART_CMD;
extern _Bool FLAG_UART_cmd_rcv;
extern _Bool FLAG_ADC_NewData;
extern void IT_Init(void);
#ifdef __cplusplus
}
#endif

#endif /* __STM32F0XX_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
