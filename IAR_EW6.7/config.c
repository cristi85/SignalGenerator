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
void Config_I2C();

void Config()
{
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  Config_GPIO();
  //Config_UART1();
  Config_TIM1();      /* Configure TIM1_CH1 as PWM output on PA8 (PWM1 Output) */
  Config_TIM2();      /* Configure TIM2 (32bit) as a free running timer for runtime measurement */
  Config_TIM3();      /* Periodic 2ms interrupt - 16bit */
  //Config_TIM6();    /* Periodic DAC triggering */
  Config_TIM14();     /* Configure TIM14 as a free running timer for Runtime measurment - 16bit*/
  //Config_ADC1_DMA();
  Config_TIM15();     /* Input capture on PA3 - TIM15_CH2 for reading FAN RPM - 16bit*/
  Config_TIM16();     /* for delay module */
  //Config_TIM17();    /* for current/power control PID task triggering */
  //Config_DAC_DMA();
  //Config_DAC();
  Config_I2C();
}

void Config_I2C()
{
  I2C_InitTypeDef I2C_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  //RCC_I2CCLKConfig(/*RCC_I2C1CLK_HSI*/RCC_I2C1CLK_SYSCLK);
  
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  //I2C_InitStructure.I2C_Timing = 0x10805E89;  // Master, Standard Mode 100Khz, 48Mhz input clock, Analog Filter Delay ON, Coefficient of Digital Filter 0, Rise 100ns, Fall 10ns
  I2C_InitStructure.I2C_Timing = 0x1080AAAA;    // 130Khz @ 48MHz input clock
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C2, &I2C_InitStructure);
  I2C_Cmd(I2C2, ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Config_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  
  /* ANALOG PINS CONFIGURATION */
  
  /* DAC OUT - analog OUTPUT */
  GPIO_InitStructure.GPIO_Pin   = DAC_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(DAC_PORT, &GPIO_InitStructure);
  
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
  /*GPIO_InitStructure.GPIO_Pin =  DEBUGPIN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DEBUGPIN_PORT, &GPIO_InitStructure);
  */
  
  /* I2C Pins - I2C2_SCL, I2C2_SDA*/
  GPIO_InitStructure.GPIO_Pin =  I2C_SDA_PIN | I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(I2C_PORT, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(I2C_PORT, GPIO_PinSource7, GPIO_AF_1);
  
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
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* GPIOA Configuration: TIM1 CH1 (PA8) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  /* Connect TIM1 Channel 1 to AF2 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 3838;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  //TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/* Configure TIM2 (32bit) as a free running timer for runtime measurement */
void Config_TIM2()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // Prescaler=48 (47+1), This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;                    // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
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

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         // Prescaler=48 (47+1), This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;                    // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
  
  TIM_Cmd(TIM14, ENABLE);
}

/* Configure TIM15_CH2 as input capture for reading FAN RPM on PA3 */
void Config_TIM15()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* TIM15 channel 2 pin (PA3) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseInitStruct.TIM_Prescaler = 47;                         
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF; 
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM15, &TIM_TimeBaseInitStruct);
  
  /* Connect TIM pins to AF0 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_0);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM15, &TIM_ICInitStructure);
  
  TIM_Cmd(TIM15, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM15, TIM_IT_CC2, ENABLE);
  
  /* Enable the TIM1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
  
  DAC_InitStructure.DAC_OutputBuffer = /*DAC_OutputBuffer_Enable*/DAC_OutputBuffer_Disable;
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
  //if(Timeout_IsTimeout1()) Errors_SetError(ERROR_ADC_INIT);
  //else Errors_ResetError(ERROR_ADC_INIT);
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
}
