#include "board.h"
#include "types.h"
#include "config.h"
#include "errors.h"
#include "timeout.h"

extern volatile u16 ADC_Conv_Tab[ADC_SCAN_CHANNELS];  /* 0 - PA5, 1 - Vref */

void Config_UART1(void);
void Config_TIM1(void);
void Config_TIM2(void);
void Config_TIM3(void);
void Config_TIM6(void);
void Config_GPIO(void);
void Config_TIM14(void);
void Config_TIM15(void);
void Config_TIM16(void);
void Config_TIM17(void);
void Config_COMP1(void);
void Config_DAC_DMA(void);
void Config_DAC(void);
void Config_ADC1_DMA(void);

void Config()
{
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  Config_GPIO();
  //Config_UART1();
  //Config_TIM1();    /* Configure TIM1_CH1 as PWM output on PA8 (PWM1 Output) */
  Config_TIM2();      /* Configure TIM2 as a free running timer for Runtime measurment */
  Config_TIM3();      /* Periodic 2ms interrupt */
  //Config_TIM6();     /* Periodic DAC triggering */
  //Config_TIM14();
  Config_ADC1_DMA();
  Config_TIM15();    /* for ADC triggering */
  Config_TIM16();    /* for delay module */
  Config_TIM17();    /* for current/power control PID task triggering */
  //Config_DAC_DMA();
  Config_DAC();
}

void Config_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  /* ANALOG PINS CONFIGURATION */
  
  /* DAC OUT - analog OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = DAC_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(DAC_PORT, &GPIO_InitStructure);
  /* CURRENT - analog INPUT */
  GPIO_InitStructure.GPIO_Pin   = CURRENT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(CURRENT_PORT, &GPIO_InitStructure);
  /* VOLTAGE - analog INPUT */
  GPIO_InitStructure.GPIO_Pin   = VOLTAGE_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(VOLTAGE_PORT, &GPIO_InitStructure);
  
  /* DIGITAL PINS CONFIGURATION */
  
  /* LCD RS - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_RS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_RS_PORT, &GPIO_InitStructure);
  /* LCD E - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_EN_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_EN_PORT, &GPIO_InitStructure);
  /* LCD D4 - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_D4_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_D4_PORT, &GPIO_InitStructure);
  /* LCD D5 - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_D5_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_D5_PORT, &GPIO_InitStructure);
  /* LCD D6 - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_D6_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_D6_PORT, &GPIO_InitStructure);
  /* LCD D7 - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin = LCD_D7_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_D7_PORT, &GPIO_InitStructure);
  /* LCD BACKLIGHT - digital OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = LCD_LIGHT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);
  /* BTN1, BTN2, BTN3, BTN4 - digital INPUT */
  GPIO_InitStructure.GPIO_Pin = BTN_INC_PIN | BTN_DEC_PIN | BTN_MODE_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BTN_PORT, &GPIO_InitStructure);

  /* USART - digital I/O */
  GPIO_InitStructure.GPIO_Pin =  USART_RX_PIN | USART_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART_PORT, &GPIO_InitStructure);
  
  /* DEBUG PIN - digital I/O */
  GPIO_InitStructure.GPIO_Pin =  DEBUGPIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DEBUGPIN_PORT, &GPIO_InitStructure);
  
  LCD_LIGHT_HIGH;
  LCD_RS(0);
  LCD_EN(0);
  LCD_D4(0);
  LCD_D5(0);
  LCD_D6(0);
  LCD_D7(0);
}

/* Configure TIM1_CH1 as PWM output on PA8 (PWM1 Output) */
void Config_TIM1()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  /* TIM2 clock enable */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // Prescaler=48 (47+1), This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 1000;                          // This parameter must be a number between 0x0000 and 0xFFFF, fclk=10k, 10000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct.TIM_Pulse = 500;                                 // Duty cycle (compared to TIM_Period)
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  
  //TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  
  // TIM1 enable output
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  
  // TIM1 enable counter
  TIM_Cmd(TIM1, ENABLE);
}

/* Configure TIM2_CH4 as PWM output on PA3 (PWM2 Output) */
void Config_TIM2()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // Prescaler=48 (47+1), This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;                    // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

/* Periodic 2ms interrupt */
void Config_TIM3()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 1999;                          // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               //This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
  /* Enable the TIM3 Update Interrupt Request */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void Config_TIM14()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM14 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 48;                         // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;                        // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
  /* enable counter */
  TIM_Cmd(TIM14, ENABLE);
}

void Config_TIM15()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // for division to 48 (1us resolution), prescaler has to be set to 48-1
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 39;                            // Overflow every (39+1) us
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM15, &TIM_TimeBaseInitStruct);
  TIM_SelectOutputTrigger(TIM15, TIM_TRGOSource_Update);
  TIM_Cmd(TIM15, ENABLE);
}

void Config_TIM16()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM16 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // for division to 48 (1us resolution), prescaler has to be set to 48-1
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;                        // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseInitStruct);
  TIM_Cmd(TIM16, ENABLE);
}

void Config_TIM17()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM17 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
  
  /* Enable the TIM17 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // for division to 48 (1us resolution), prescaler has to be set to 48-1
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 499;                           // Overflow every (499+1) us
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM17, &TIM_TimeBaseInitStruct);
  TIM17->CNT = 0;
  TIM_ClearITPendingBit(TIM17, TIM_IT_Update);
  TIM_Cmd(TIM17, ENABLE);
  /* Enable the TIM3 Update Interrupt Request */
  TIM_ITConfig(TIM17, TIM_IT_Update, ENABLE);
}

void Config_UART1()
{
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable USART1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  /* Enable the USART1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure GPIO pin alternate function UART1 RX TX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  /* =================================== */
  USART_DeInit(USART1);
  USART_InitStruct.USART_BaudRate = 19200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

void Config_DAC()
{
  DAC_InitTypeDef DAC_InitStructure;

  //PA4 DAC Output
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  
  /*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       //DAC Output
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);*/
  
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  DAC_Cmd(DAC_Channel_1, ENABLE);
}

void Config_DAC_DMA()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef DAC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  //PA4 DAC Output
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       //DAC Output
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  /* DMA1 Channel3 Config */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->DHR12R1);
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(/*&Sinus12bit130*/0);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 130;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel3, ENABLE);
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void Config_TIM6()  /* Periodic DAC triggering */
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM6 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;                          // F_timer = 48MHz
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // UP Counter
  TIM_TimeBaseInitStruct.TIM_Period = 36923;                         // 10Hz for 130 signal values
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // Clock Division = 1
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_DMACmd(TIM6, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM6, DISABLE);
}

void Config_ADC1_DMA()
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(&ADC_Conv_Tab);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_SCAN_CHANNELS;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* Enable the DMA channel 1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable Transfer Complete IT notification for DMA1 Channel 1 */
  DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T15_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_239_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_239_5Cycles);
  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_55_5Cycles); 
  ADC_VrefintCmd(ENABLE);
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN flag */
  Timeout_SetTimeout1(50);   //Set timeout1 to 50ms
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) && !Timeout_IsTimeout1()); 
  if(Timeout_IsTimeout1()) Errors_SetError(ERROR_ADC_INIT);
  else Errors_ResetError(ERROR_ADC_INIT);
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
}
