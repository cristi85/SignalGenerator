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
#include "config.h"

#define UARTCMDLEN        (u8)32
#define BTN_DEPRESSED     (u8)0
#define BTN_PRESSED       (u8)1
   
#define BTN_DELAY_300ms   (u8)150   /* 300ms */
#define BTN_DELAY_1000ms  (u16)500  /* 1s */
#define BTN_DELAY_2500ms  (u16)1250 /* 2.5s */
#define BTN_DELAY_5000ms  (u16)2500 /* 5s */
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern volatile u8 BTN_MODE_DEB_STATE;
extern volatile u8 BTN_DEC_DEB_STATE;
extern volatile u8 BTN_INC_DEB_STATE;
extern _Bool BTN_MODE_DELAY_FLAG;
extern _Bool BTN_DEC_DELAY_FLAG;
extern _Bool BTN_INC_DELAY_FLAG;
extern u16 BTN_MODE_press_timer;
extern u16 BTN_DEC_press_timer;
extern u16 BTN_INC_press_timer;

extern bool LCD_UPDATE_LIMIT_FLAG;

extern _Bool FLAG_10ms;
extern _Bool FLAG_100ms;
extern _Bool FLAG_250ms;
extern _Bool FLAG_500ms;
extern _Bool FLAG_1000ms;
extern bool FLAG_PID_500us;
extern _Bool FLAG_ADC_NewData;
extern void IT_Init(void);

/* ADC measurements */
extern volatile u16 ADC_Conv_Tab_Avg[ADC_SCAN_CHANNELS];
#define ADC_VREF     ADC_Conv_Tab_Avg[0]
#define ADC_VOLTAGE  ADC_Conv_Tab_Avg[1]
#define ADC_CURRENT  ADC_Conv_Tab_Avg[2]

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0XX_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
